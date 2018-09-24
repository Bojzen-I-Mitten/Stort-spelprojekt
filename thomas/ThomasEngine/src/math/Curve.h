#pragma once

namespace ThomasEngine
{
	public ref class Curve
	{
	public:
		Curve();
		System::Collections::Generic::List<System::Windows::Point>^ points;
		bool expandedInPropertyGrid = false;

	private:
		double Interpolate(int indexLP, double x);

		double BezierFunction(int indexLP, double t);

		int GetLinePointIndexToLeft(double x);

	public:
		double GetYFromX(double x);
	};
}

