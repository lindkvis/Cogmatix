//
//  Machine.cpp
//  LibCogmatix
//
//  Created by Gaute Lindkvist on 12/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#include "Machine.h"
#include "Part.h"

namespace LibCogmatix
{
    GearList Machine::gears()
    { 
        GearList gears;
        findDescendantsOfType<ParametricSpurGearPart> (this, gears);
        return gears;
    }
}