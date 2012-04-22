#include "StdAfx.h"
#include "Axis.h"
#include "Units.h"
#include "Machine.h"

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
    
    MachineNode::Compatibility Axis::isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const
    {
        if (slave == this) // An object can't move itself
            return Self;
        else if (chain.find(slave) != chain.end())
            return AlreadyMoving;
        
        return NotDetermined;
    }
    
    bool Axis::moveOthers (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked)
    {
        bool bOK=true;
        // Do some magic to move other gears
        std::list<Machine*> machines;
        findParentsOfType<Machine>(this, machines);
        assert (!machines.empty());
        if (machines.empty())
            return false;
        Machine* machine = machines.front();
        if (machine)
        {
            chain.insert(this);
            AxisList axes = machine->axes();
            foreach (AxisDistance slave, axes)
            {
                if (bOK && slave.axis)
                {
                    Compatibility compat = isCompatible(chain, slave.axis);
                    switch (compat)
                    {
                        case Compatible:
                            bOK = slave.axis->move(delta, chain, this, blocked);
                            break;
                        case OnAxis:
                            bOK = slave.axis->move(delta, chain, this, blocked);
                            break;
                        case Conflict:
                            bOK = false;
                            break;
                        default:
                            // do nothing
                            break;
                    }
                }
            }
        }
        // Call parent moveTo method
        //        if (!blocked)
        //  RotaryAxis::moveTo(_value + delta);
        return bOK;
        
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
        moveOthers(delta, chain, master, blocked);
		return blocked ? false : moveTo(_value + delta);
	}

    MachineNode::Compatibility RotaryAxis::isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const
    {
        Compatibility axisCompatibility = Axis::isCompatible(chain, slave);
        if (axisCompatibility != NotDetermined) // if already determined, just return it
            return axisCompatibility;

        const RotaryAxis* axis = dynamic_cast<const RotaryAxis*> (slave);
        if (axis)
        {
            Vec axisOwn = worldAxis();
            Vec axisOther = axis->worldAxis();
            bool bCompatibleAxis = bCompatibleAxis = (axisOwn - axisOther).length() < EPSILON;

            // Positions along the world axis
            double n1val = worldPosition() * axisOwn;
            double n2val = axis->worldPosition() * axisOther;
            
            // Positions in plane (2D)
            Vec posOwn = worldPosition() - axisOwn * n1val;
            Vec posOther = axis->worldPosition() - axisOther * n2val;
                
            // total distance between them
            double distance = (posOwn - posOther).length();
                
            if (bCompatibleAxis && distance < AXIS_TOLERANCE && (worldPosition() - axis->worldPosition()).length() < 10.) // on same axis
                return OnAxis; 
            else if (bCompatibleAxis && distance < SNAP_TOLERANCE)
                return CanSnapTo;
        }
        return NotDetermined; // not correct, but we don't have any other types of gears yet.
    }
    
    bool RotaryAxis::snapTo (const MachineNode* masterNode)
    {
        
        const RotaryAxis* master = dynamic_cast<const RotaryAxis*>(masterNode);
        if (!master)
            return RotaryAxis::snapTo(masterNode);
        Compatibility compat = masterNode->isCompatible(std::set<const MachineNode*>(), this);
        if (compat == CanSnapTo || compat == Conflict)
        {
            Vec axisOwn = worldAxis();
            Vec axisOther = master->worldAxis();
            // Positions along the world axis
            Vec posOwn = worldPosition();
            Vec posOther = master->worldPosition();
            double n1val = posOwn * axisOwn;
            double n2val = posOther * axisOther;
            
            // Position vectors
            posOwn -= axisOwn * n1val;
            posOther -= axisOther * n2val;
            Vec vecDist = posOther - posOwn;
            // total distance between them
            double distance = vecDist.length();
            if (distance < EPSILON)
            {
                vecDist = Vec(1., 0., 0.);
            }
            
            osg::Matrixd M = worldMatrix();
            osg::Matrixd MI = osg::Matrixd::inverse(M);
            
            Vec newPosOwn = worldPosition() + vecDist;
            setOrigin (newPosOwn * MI);
            reset();
            return true;
        }
        return false;
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
