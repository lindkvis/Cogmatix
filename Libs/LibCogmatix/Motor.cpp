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
//	    static double accelTimer=0.;
		if (isRunning() && getAxis())
		{
            //            double RPS = std::min (_RPS, (accelTimer * _RPS)); // hardcoded acceleration. Will reach full speed in one second
			std::set<const MachineNode*> chain;
			_blocked = !getAxis()->move(dsecs * _RPS, chain, this, _blocked);
		}
        //      accelTimer+= dsecs;
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
