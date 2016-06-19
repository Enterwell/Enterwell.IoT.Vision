#include "pch.h"
#include "Vector3d.h"

using namespace Enterwell_IoT_Vision_Algorithm;

Vector3d::Vector3d(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
double Vector3d::GetX() {
	return this->x;
}
double Vector3d::GetY() {
	return this->y;
}
double Vector3d::GetZ() {
	return this->z;
}