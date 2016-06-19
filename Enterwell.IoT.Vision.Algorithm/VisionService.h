#pragma once
#include <Vector3d.h>
#include <Visodo.h>
namespace Enterwell_IoT_Vision_Algorithm 
{
	using namespace Windows::UI::Xaml::Media::Imaging;

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class VisionService sealed
	{
	public:
		VisionService();
		Vector3d^ CalculateTranslationAndRotation(WriteableBitmap^ bitmap);
	private:
		Visodo* m_visodo;
	};
}

