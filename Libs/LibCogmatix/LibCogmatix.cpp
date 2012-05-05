//
//  LibCogmatix.cpp
//  LibCogmatix
//
//  Created by Gaute Lindkvist on 12/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#include "LibCogmatix.h"
#include <osg/Material>
#include <osg/Vec3>
#include <osg/Array>
#include <osg/Geometry>
#include <osg/Geode>

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
    
    osg::Node* createBase(const osg::Vec3f& center,float radius)
    {
#define numTilesX 10
#define numTilesZ 10
        
        float width = 2*radius;
        float height = 2*radius;
        
        osg::Vec3f v000(center - osg::Vec3f(width*0.5f,0.0f, height*0.5f));
        osg::Vec3f dx(osg::Vec3f(width/((float)numTilesX),0.0,0.0f));
        osg::Vec3f dz(osg::Vec3f(0.0f,0.0f, height/((float)numTilesZ)));
        
        osg::Vec2f tdx = osg::Vec2f(1.f, 0.f);
        osg::Vec2f tdy = osg::Vec2f(0.f, 1.f);
        
        // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
        osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
        int iz;
        for(iz=0;iz<=numTilesZ;++iz)
        {
            for(int ix=0;ix<=numTilesX;++ix)
            {
                coords->push_back(v000+dx*(float)ix+dz*(float)iz);
                texcoords->push_back(tdx*(float)ix+tdy*(float)iz);
            }
        }
        
        //Just two colours - black and white.
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
        colors->push_back(osg::Vec4(1.f,1.f,1.f,1.0f)); // grey
        int numColors=colors->size();
        
        
        int numIndicesPerRow=numTilesX+1;
        int numIndicesPerColumn=numTilesZ+1;
        
        osg::UIntArray* coordIndices = new osg::UIntArray; // assumes we are using less than 256 points...
        osg::UIntArray* colorIndices = new osg::UIntArray;
        osg::ref_ptr<osg::Vec2Array> texpoints = new osg::Vec2Array;
        for(iz=0;iz<numTilesZ;++iz)
        {
            for(int ix=0;ix<numTilesX;++ix)
            {
                // four vertices per quad.
                coordIndices->push_back(ix+iz*numIndicesPerRow);
                osg::Vec2 v = (*texcoords)[coordIndices->back()];
                texpoints->push_back(v);
                
                coordIndices->push_back((ix+1)+iz*numIndicesPerRow);
                v = (*texcoords)[coordIndices->back()];
                texpoints->push_back((*texcoords)[coordIndices->back()]);
                
                coordIndices->push_back((ix+1)+(iz+1)*numIndicesPerRow);
                v = (*texcoords)[coordIndices->back()];
                texpoints->push_back((*texcoords)[coordIndices->back()]);
                
                coordIndices->push_back(ix+(iz+1)*numIndicesPerRow);
                v = (*texcoords)[coordIndices->back()];
                texpoints->push_back((*texcoords)[coordIndices->back()]);
                
                // one color per quad
                colorIndices->push_back((ix+iz)%numColors);
            }
        }
        
        
        // set up a single normal
        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3f(0.0f,-1.0f,0.0f));
        
        
        osg::Geometry* geom = new osg::Geometry;
        geom->setVertexArray(coords);
        geom->setVertexIndices(coordIndices);
        geom->setTexCoordArray(0, texpoints);
        geom->setColorArray(colors);
        geom->setColorIndices(colorIndices);
        geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
        
        geom->setNormalArray(normals);
        geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
        
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));
        
        osg::Geode* geode = new osg::Geode;
        geode->addDrawable(geom);
        
        return geode;
    }
}