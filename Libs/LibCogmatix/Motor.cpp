#include "StdAfx.h"
#include "Motor.h"

namespace LibCogmatix
{
	Motor::Motor(NodeID ID, double RPM, const Vec& axisVector, const Vec& origin, double axisDiameter, double axisLength) : RotaryAxis(ID, axisVector, origin, axisDiameter, axisLength, 0., 0., 0.), _RPS(RPM/60.), _blocked(false), _isRunning(false)
	{
	}


	Motor::~Motor(void)
	{
	}

	void Motor::tick(double dsecs)
	{
		if (isRunning())
		{
			std::set<const MachineNode*> chain;
			_blocked = !moveOthers(dsecs * _RPS, chain, this, _blocked);
		}
	}
    
    // Valid actions
    Actions Motor::validActions() const
    {
        Actions actions = MachineNode::validActions();
        if (isRunning())
            actions.push_back(Action("stop"));
        else
            actions.push_back(Action("start"));
        return actions;
    }
    
    ActionResult Motor::perform(CoString action, const ActionArgs& args)
    {
        if (action == "start")
            start();
        else if (action == "stop")
            stop();
        else
            return MachineNode::perform(action, args);
        
        return ActionResult();
    }
}
