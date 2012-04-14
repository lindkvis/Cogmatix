#include "StdAfx.h"
#include "Motor.h"

namespace LibCogmatix
{
	Motor::Motor(NodeID ID, double RPM) : TMachineNode(ID), _RPS(RPM/60.), _blocked(false), _isRunning(false)
	{
	}


	Motor::~Motor(void)
	{
	}

	void Motor::tick(double dsecs)
	{
		if (isRunning() && getAxis())
		{
			std::set<const MachineNode*> chain;
			_blocked = !getAxis()->move(dsecs * _RPS, chain, this, _blocked);
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
