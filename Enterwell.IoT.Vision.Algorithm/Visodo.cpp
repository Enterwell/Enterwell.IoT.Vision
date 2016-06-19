/*

The MIT License

Copyright (c) 2015 Avi Singh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include "pch.h"
#include "Visodo.h"

using namespace cv;
using namespace std;
using namespace Enterwell_IoT_Vision_Algorithm;
void Visodo::featureTracking(Mat img_old, Mat img_new, vector<Point2f>& points_old, vector<Point2f>& points_new, vector<uchar>& status) {

	//this function automatically gets rid of points for which tracking fails

	vector<float> err;
	Size winSize = Size(21, 21);
	TermCriteria termcrit = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.01);

	calcOpticalFlowPyrLK(img_old, img_new, points_old, points_new, status, err, winSize, 3, termcrit, 0, 0.001);

	//getting rid of points for which the KLT tracking failed or those who have gone outside the frame
	int indexCorrection = 0;
	for (int i = 0; i<status.size(); i++)
	{
		Point2f pt = points_new.at(i - indexCorrection);
		if ((status.at(i) == 0) || (pt.x<0) || (pt.y<0)) {
			if ((pt.x<0) || (pt.y<0)) {
				status.at(i) = 0;
			}
			points_old.erase(points_old.begin() + (i - indexCorrection));
			points_new.erase(points_new.begin() + (i - indexCorrection));
			indexCorrection++;
		}

	}

}


void Visodo::featureDetection(Mat img_1, vector<Point2f>& points_old) {   //uses FAST as of now, modify parameters as necessary
	vector<KeyPoint> keypoints_1;
	int fast_threshold = 20;
	bool nonmaxSuppression = true;
	FAST(img_1, keypoints_1, fast_threshold, nonmaxSuppression);
	KeyPoint::convert(keypoints_1, points_old, vector<int>());
}

double Visodo::getAbsoluteScale(int frame_id, int sequence_id, double z_cal) {

	//string line;
	//int i = 0;
	//ifstream myfile("/home/avisingh/Datasets/KITTI_VO/00.txt");
	//double x = 0, y = 0, z = 0;
	//double x_prev, y_prev, z_prev;
	//if (myfile.is_open())
	//{
	//	while ((getline(myfile, line)) && (i <= frame_id))
	//	{
	//		z_prev = z;
	//		x_prev = x;
	//		y_prev = y;
	//		std::istringstream in(line);
	//		//cout << line << '\n';
	//		for (int j = 0; j<12; j++) {
	//			in >> z;
	//			if (j == 7) y = z;
	//			if (j == 3)  x = z;
	//		}

	//		i++;
	//	}
	//	myfile.close();
	//}

	//else {
	//	cout << "Unable to open file";
	//	return 0;
	//}

	//return sqrt((x - x_prev)*(x - x_prev) + (y - y_prev)*(y - y_prev) + (z - z_prev)*(z - z_prev));
	return 1;
}

Point3d Visodo::GetRotationAndTranslation(Mat img_new) {
	m_new = img_new;

	Mat img_1, img_2;
	Mat R_f, t_f;

	double scale = 1.00;

	if (countNonZero(m_old) == 0) {
		m_old = img_new;
		featureDetection(m_old, points_old);
		return Point3d();
	}

	// we work with grayscale images
	cvtColor(m_old, img_1, COLOR_BGR2GRAY);
	cvtColor(m_new, img_2, COLOR_BGR2GRAY);

	vector<uchar> status;

	double focal = 718.8560;
	cv::Point2d pp(607.1928, 185.2157);
	//recovering the pose and the essential matrix
	Mat E, R, t, mask;

	//cout << numFrame << endl;

	featureTracking(img_1, img_2, points_old, points_new, status);

	E = findEssentialMat(points_new, points_old, focal, pp, RANSAC, 0.999, 1.0, mask);
	recoverPose(E, points_new, points_old, R, t, focal, pp, mask);

	Mat prevPts(2, points_old.size(), CV_64F), currPts(2, points_new.size(), CV_64F);


	for (int i = 0; i<points_old.size(); i++) {   //this (x,y) combination makes sense as observed from the source code of triangulatePoints on GitHub
		prevPts.at<double>(0, i) = points_old.at(i).x;
		prevPts.at<double>(1, i) = points_old.at(i).y;

		currPts.at<double>(0, i) = points_new.at(i).x;
		currPts.at<double>(1, i) = points_new.at(i).y;
	}

	scale = getAbsoluteScale(1, 0, t.at<double>(2));

	//cout << "Scale is " << scale << endl;

	if ((scale>0.1) && (t.at<double>(2) > t.at<double>(0)) && (t.at<double>(2) > t.at<double>(1))) {

		t_f = t_f + scale*(R_f*t);
		R_f = R*R_f;

	}

	else {
		//cout << "scale below 0.1, or incorrect translation" << endl;
	}

	// lines for printing results
	// myfile << t_f.at<double>(0) << " " << t_f.at<double>(1) << " " << t_f.at<double>(2) << endl;

	// a redetection is triggered in case the number of feautres being trakced go below a particular threshold
	if (points_old.size() < MIN_NUM_FEAT) {
		//cout << "Number of tracked features reduced to " << prevFeatures.size() << endl;
		//cout << "trigerring redection" << endl;
		featureDetection(img_1, points_old);
		featureTracking(img_1, img_2, points_old, points_new, status);

	}

	Point3d result(t_f.at<double>(0), t_f.at<double>(1), t_f.at<double>(2));

	m_old = m_new.clone();
	points_old = points_new;

	return result;
}
