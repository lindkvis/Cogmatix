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
    AxisList Machine::axes()
    { 
        AxisList nodes;
        findDescendantsOfType<Axis> (dynamic_cast<osg::Node*> (this), nodes);
        return nodes;
    }
}