//
//  Machine.cpp
//  LibCogmatix
//
//  Created by Gaute Lindkvist on 12/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#include "Machine.h"
#include "GearPart.h"

namespace LibCogmatix
{
    AxisList Machine::axes(Axis* master)
    { 
        AxisList nodes;
        std::list<Axis*> axes;
        findDescendantsOfType<Axis> (dynamic_cast<osg::Node*> (this), axes);
        // insert into ordered set
        foreach (Axis* axis, axes)
        {
            if (axis != master)
                nodes.insert (AxisDistance(axis, master));
        }
        return nodes;
    }
}