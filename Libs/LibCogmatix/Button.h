//
//  Button.h
//  Cogmatix
//
//  Created by Chris Chilvers on 28/04/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#pragma once

#include "osgWidget/Label"

namespace LibCogmatix
{
    using namespace osgWidget;
    
    class Button : public Label
    {
    public:
        Button(const std::string& name, const std::string& label);
        virtual bool mouseEnter(double, double, const WindowManager*);
        virtual bool mouseLeave(double, double, const WindowManager*);
        virtual bool mousePush(double, double, const WindowManager*);
        virtual bool mouseRelease(double, double, const WindowManager*);
        virtual bool mouseDrag(double, double, const WindowManager*);
    };
}
