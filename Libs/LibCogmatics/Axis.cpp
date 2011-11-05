#include "StdAfx.h"
#include "Axis.h"

namespace LibCogmatics
{
#pragma region Axis
	// Initialise static diagnostics
	Axis::Diagnostics Axis::s_diagnostics = Diagnostics();
#pragma endregion

#pragma region RotaryAxis
	bool RotaryAxis::moveTo (float newValue)
	{
		if (newValue >= _min && newValue <= _max)
		{
			_matrix.postMult(Matrix::rotate(newValue-_value, _axisVector));
			_value = newValue;	
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
#pragma endregion

#pragma region LinearAxis
	bool LinearAxis::moveTo (float newValue)
	{
		if (newValue >= _min && newValue <= _max)
		{
			_matrix.postMult(Matrix::translate(_axisVector * (newValue - _value)));
			_value = newValue;
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
#pragma endregion
}