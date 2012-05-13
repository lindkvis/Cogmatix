//
//  OsgPlugins.h
//  Cogmatix
//
//  Created by Gaute Lindkvist on 03/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//
#pragma once 

#include <osgViewer/GraphicsWindow>
#include <osgDB/Registry>

#ifdef TARGET_OS_IPHONE
//USE_OSGPLUGIN(obj)
//USE_OSGPLUGIN(ive)
//USE_OSGPLUGIN(osg)
//USE_OSGPLUGIN(rgb)
//USE_OSGPLUGIN(glsl)
//USE_OSGPLUGIN(bmp)
USE_OSGPLUGIN(imageio)
USE_OSGPLUGIN(freetype)
#endif