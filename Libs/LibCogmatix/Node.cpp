#include "stdafx.h"
#include "Node.h"
#include "Factory.h"

namespace LibCogmatix
{
    Actions MachineNode::validActions () const
    {
        Actions actions;
        actions.push_back(Action("remove"));
        actions.push_back(Action("copy"));
        return actions;
    }
    
    ActionResult MachineNode::perform (CoString action, const ActionArgs& args)
    {
        if (action == "remove")
            return Factory::get()->removeNode(_ID);
        else if (action == "copy")
            return Factory::get()->copyNode(_ID);
        return ActionResult();
    }
}