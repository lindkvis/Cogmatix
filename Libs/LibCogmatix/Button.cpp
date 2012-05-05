//
//  Button.cpp
//  Cogmatix
//
//  Created by Chris Chilvers on 28/04/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#include "stdafx.h"
#include "Button.h"

using namespace osgWidget;
using namespace LibCogmatix;

const Color background = Color(0.4f, 0.4f, 0.4f, 0.3f);
const Color highlight  = Color(0.8f, 0.8f, 0.8f, 1.0f);

Button::Button(const std::string& name, const std::string& label) :
    Label(name, label)
{
    getText()->setBackdropImplementation(osgText::Text::POLYGON_OFFSET);
    setFont("Vera.ttf");
    setFontSize(15);
    setColor(background);
    setCanFill(true);
    //    setShadow(0.1f);
    addSize(200.0f, 100.0f);
    setEventMask(EVENT_MASK_MOUSE_CLICK |
                 EVENT_MASK_MOUSE_MOVE |
                 EVENT_MASK_MOUSE_DRAG);
}

bool Button::mouseEnter(double, double, const WindowManager*)
{
    setColor(highlight);
    return true;
}

bool Button::mouseLeave(double, double, const WindowManager*)
{
    setColor(background);
    return true;
}

bool Button::mousePush(double, double, const WindowManager*)
{
    setColor(highlight);
    return true;
}

bool Button::mouseRelease(double, double, const WindowManager*)
{
    setColor(background);
    return true;
}

bool Button::mouseDrag(double, double, const WindowManager*)
{
    return true;
}
