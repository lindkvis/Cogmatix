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
		static double blockTimer=0.;
        static double accelTimer=0.;
		static double RPS = 0.25*_RPS;
		if (!_blocked)
			RPS = _RPS; // Make sure it is always forwards at full speed if we're not blocked
		if (isRunning() && getAxis())
		{
            RPS = std::min (RPS, (accelTimer * RPS)); // hardcoded acceleration. Will reach full speed in one second
			std::set<const MachineNode*> chain;
			if (!getAxis()->move(dsecs * RPS, chain, this))
			{
				_blocked=true;
				if (blockTimer > 0.25)
				{
					blockTimer = 0.;
					RPS *= -1;
				}
				else
					blockTimer += dsecs;
			}
		}
        accelTimer+= dsecs;
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
