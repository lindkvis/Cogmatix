#include "StdAfx.h"
#include "Axis.h"
#include "Units.h"

namespace LibCogmatix
{
#pragma region Axis
	// Initialise static diagnostics
	Axis::Diagnostics Axis::s_diagnostics = Diagnostics();
    
    Vec Axis::worldAxis() const
    {
        return worldMatrix().getRotate() * _axisVector; // do rotation only
    }
    Vec Axis::worldPosition() const
    {
        return _position * worldMatrix();
    }
    
    osg::Matrixd Axis::worldMatrix() const
    {
        // a node may have multiple parents. Only deal with first one for now.
        if (getNumParents() > 0 && !getParent(0)->getWorldMatrices().empty())
            return getParent(0)->getWorldMatrices().front();
        return osg::Matrixd();
    }
    
    osg::BoundingSphere Axis::worldBound() const
    {
        return transformBoundingSphere (getParent(0)->getWorldMatrices().front(), getBound());
    }    
#pragma endregion

#pragma region RotaryAxis
	bool RotaryAxis::moveTo (float newValue)
	{
		if ((fabs(_min) < EPSILON && fabs(_max) < EPSILON) || (newValue >= _min && newValue <= _max))
		{
			//osg::Quat quat; quat.makeRotate(newValue-_value, _axisVector);
			_attitude = _attitude * osg::Quat(newValue-_value, _axisVector);
			//postMult(Matrix::rotate(newValue-_value, _axisVector));
			_value = newValue;	
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
	bool RotaryAxis::move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked)
	{
		return blocked ? false : moveTo(_value + delta);
	}

#pragma endregion

#pragma region LinearAxis
	bool LinearAxis::moveTo (float newValue)
	{
		if (newValue >= _min && newValue <= _max)
		{
			_position += _axisVector * (newValue - _value);
			//postMult(Matrix::translate(_axisVector * (newValue - _value)));
			_value = newValue;
			return true;
		}
		else
			s_diagnostics[LimitsExceeded] = true;
		return false;
	}
	bool LinearAxis::move(float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked)
	{
		return blocked ? false : moveTo(_value + delta);
	}
#pragma endregion
}
