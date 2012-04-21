//
//  Action.h
//  LibCogmatix
//
//  Created by Gaute Lindkvist on 12/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//
#pragma once

#include <vector>
#include <string>
#include <boost/any.hpp>

#include "LibCogmatix.h"

namespace LibCogmatix
{
    using boost::any_cast;
    typedef boost::any ActionArg;
    typedef boost::any ActionResult;
    typedef std::vector<ActionArg> ActionArgs;
    
    struct Action
    {
        std::string name; ///<Name of the action>
        Action () {}                
        Action (CoString actionName) : name(actionName) {} 
        
        bool operator==(Action other) const { return name == other.name; }
        bool operator< (Action other) const { return name < other.name; }
        bool operator> (Action other) const { return name > other.name; }
        bool operator==(CoString actionName) const { return name == actionName; }
    };
    typedef std::vector<Action> Actions;
}