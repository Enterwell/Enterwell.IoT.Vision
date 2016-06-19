#pragma once
namespace Enterwell_IoT_Vision_Algorithm
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Vector3d sealed
	{
	public:
		Vector3d(double x, double y, double z);
		double GetX();
		double GetY();
		double GetZ();
	private:
		double x, y, z;
	};
}
