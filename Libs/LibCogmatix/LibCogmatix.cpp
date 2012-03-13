//
//  LibCogmatix.cpp
//  LibCogmatix
//
//  Created by Gaute Lindkvist on 12/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#include "LibCogmatix.h"

namespace LibCogmatix 
{
    osg::BoundingSphere transformBoundingSphere( const osg::Matrixf& m, const osg::BoundingSphere& sphere )
    {
        osg::BoundingSphere::vec_type xdash = sphere._center;
        xdash.x() += sphere._radius;
        xdash = xdash * m;
        
        osg::BoundingSphere::vec_type ydash = sphere._center;
        ydash.y() += sphere._radius;
        ydash = ydash * m;
        
        osg::BoundingSphere::vec_type zdash = sphere._center;
        zdash.z() += sphere._radius;
        zdash = zdash * m;
        
        osg::BoundingSphere newSphere;
        newSphere._center = sphere._center * m;
        
        xdash -= newSphere._center;
        osg::BoundingSphere::value_type len_xdash = xdash.length();
        
        ydash -= newSphere._center;
        osg::BoundingSphere::value_type len_ydash = ydash.length();
        
        zdash -= newSphere._center;
        osg::BoundingSphere::value_type len_zdash = zdash.length();
        
        newSphere._radius = len_xdash;
        if( newSphere._radius < len_ydash )
            newSphere._radius = len_ydash;
        if( newSphere._radius < len_zdash )
            newSphere._radius = len_zdash;
        
        return( newSphere );
    }
}