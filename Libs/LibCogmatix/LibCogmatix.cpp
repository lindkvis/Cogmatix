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
    
    void getEulerFromQuat(osg::Quat q, double& a, double& b, double& c)     
    { 
        double limit = 0.499999; 
        
        double sqx = q.x()*q.x(); 
        double sqy = q.y()*q.y(); 
        double sqz = q.z()*q.z(); 
        
        double t = q.x()*q.y() + q.z()*q.w(); 
        
        if (t>limit) // gimbal lock ? 
        { 
            a = 0; 
            b = osg::PI_2; 
            c = 2 * atan2(q.x(),q.w()); 
        } 
        
        else if (t<-limit) 
        { 
            a = 0; 
            b = - osg::PI_2; 
            c = -2 * atan2(q.x(),q.w()); 
        } 
        else 
        { 
            a = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz); 
            b = asin(2*t); 
            c = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz); 
        } 
    }
}