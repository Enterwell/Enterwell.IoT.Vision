#include "pch.h"
#include "VisionService.h"
#include <ppltasks.h>
#include <wrl.h>
#include <robuffer.h>

using namespace Windows::Storage;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace Enterwell_IoT_Vision_Algorithm;

VisionService::VisionService()
{
	this->m_visodo = new Visodo();
}
Vector3d^ VisionService::CalculateTranslationAndRotation(WriteableBitmap^ bitmap)
{
	// Converts WriteableBitmap to cv::Mat
	/*IBuffer^ buffer = bitmap->PixelBuffer;
	auto reader = ::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
	BYTE *extracted = new BYTE[buffer->Length];
	reader->ReadBytes(Platform::ArrayReference<BYTE>(extracted, buffer->Length));
	Mat img_image(bitmap->PixelHeight, bitmap->PixelWidth, CV_8UC4, extracted);
	cvtColor(img_image, img_image, CV_RGBA2BGRA);
*/
	Mat mat(bitmap->PixelHeight, bitmap->PixelWidth, CV_8UC4, (void*)(bitmap->PixelBuffer));

	// Set image to new image
	auto result = this->m_visodo->GetRotationAndTranslation(mat);

	// return Point3d
	return ref new Vector3d(result.x, result.y, result.z);
}

