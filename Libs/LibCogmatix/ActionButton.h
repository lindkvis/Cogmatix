//
//  ActionButton.h
//  Cogmatix
//
//  Created by Chris Chilvers on 28/04/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#pragma once

#include <assert.h>
#include <osgText/String>
#include <osgWidget/WindowManager>
#include "Button.h"
#include "EventHandler.h"

namespace LibCogmatix
{
    class ActionButton : public Button
    {
    public:
        ActionButton(EventHandler* handler, CoString action) :
            Button("", action),
            _handler(handler),
            _action(action)
        {
            assert(handler);
        }
        
        virtual bool mousePush(double, double, const osgWidget::WindowManager*)
        {
            _handler->dispatchAction(_action);
            return true;
        }
        CoString action() const { return _action; }
    protected:
        osg::ref_ptr<EventHandler> _handler;
        std::string _action;
    };
}
