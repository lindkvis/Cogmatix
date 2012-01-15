#include "StdAfx.h"
#include "Axis.h"
#include "Units.h"

namespace LibCogmatix
{
#pragma region Axis
	// Initialise static diagnostics
	Axis::Diagnostics Axis::s_diagnostics = Diagnostics();
#pragma endregion

#pragma region RotaryAxis
	bool RotaryAxis::moveTo (float newValue)
	{
		if ((fabs(_min) < epsilon && fabs(_max) < epsilon) || (newValue >= _min && newValue <= _max))
		{
			postMult(Matrix::rotate(newValue-_value, _axisVector));
			_value = newValue;	
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
	bool RotaryAxis::move (float delta)
	{
		return moveTo(_value + delta);
	}

#pragma endregion

#pragma region LinearAxis
	bool LinearAxis::moveTo (float newValue)
	{
		if (newValue >= _min && newValue <= _max)
		{
			postMult(Matrix::translate(_axisVector * (newValue - _value)));
			_value = newValue;
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
	bool LinearAxis::move(float delta)
	{
		return moveTo(_value + delta);
	}
#pragma endregion
}