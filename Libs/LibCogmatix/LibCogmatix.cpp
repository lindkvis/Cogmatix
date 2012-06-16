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

#include <osgGA/StateSetManipulator>
//#include <osgShadow/ShadowSettings>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgFX/Cartoon>
#include <osgFX/Outline>

#include "Factory.h"
#include "Clock.h"
#include "Light.h"
#include "Node.h"
#include "GearPart.h"

namespace LibCogmatix 
{
    Machine::Ptr createTestMachine(osg::Group* root)
    {
        
        Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");
        BoxMotor::Ptr motor = Factory::get()->CreateBoxMotor(20, Vec(0., 1., 0.), Vec(0., 10., 0.), Vec(5., 5., 5.), 1.0, 7.5);
        machine->addChild(motor);
        
        
        ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(0., 5., 0.), 40, 1.5, 1.0, 0.3, 0., PI/2., red);
        ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(7.5, 5., 0.),10, 1.0, 0.5, 0.3, 0., PI/2., blue);
        ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 5., 0.), 10, 1.0, 0.5, 0.3, 0., PI/2., green);
        ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 5., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/2., yellow);
        ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 5., 11.7), 20, 1.0, 0.5, 0.3, 0., PI/2., purple);
        
        //ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, -2., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
        //ParametricSpurGearPart::Ptr gear6 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 0., 1.), Vec(10.5, -6., 10.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
        machine->addChild(gear);
        machine->addChild(gear2);
        machine->addChild(gear3);
        machine->addChild(gear4);
        machine->addChild(gear5);
       
        osg::ref_ptr<osg::Node> plane_geode = createBase(Vec(0., 20., 0.), 100);
        osg::StateSet* planeState = plane_geode->getOrCreateStateSet();
        
        osg::ref_ptr<osg::Texture2D> planeTex = new osg::Texture2D(osgDB::readImageFile("paper_tile.jpg"));
        //planeTex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
        //        planeTex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
        planeTex->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);    
        planeTex->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);    
        planeState->setTextureAttributeAndModes( 0, planeTex , osg::StateAttribute::ON);    
        //machine->addChild(gear6);
        Clock::get()->add(motor);
        
        osg::StateSet* gearState = machine->getOrCreateStateSet();
        motor->start();

#ifndef TARGET_OS_IPHONEdd
        osg::ref_ptr<osgFX::Cartoon> cartoon = new osgFX::Cartoon();
        cartoon->addChild(machine);
        osg::ref_ptr<osg::Group> world = createShadowedScene(machine, cartoon, plane_geode, Vec(30., -200., 30.));
#else
        osg::ref_ptr<osg::Group> nullEffect = new osg::Group();
        nullEffect->addChild(machine);
        osg::ref_ptr<osg::Group> world = createShadowedScene(machine, nullEffect, plane_geode, Vec(30., -200., 30.));        
#endif
        root->addChild(world);
        return machine;
    }
    
    osg::ref_ptr<osg::Group> createShadowedScene(Machine::Ptr machine, osg::ref_ptr<osg::Node> shadowee, osg::ref_ptr<osg::Node> shadowed, const Vec& lightPosition)
    {   
#ifdef TARGET_OS_IPHONE
        osg::ref_ptr<osg::Group> world = new osg::Group;  
#else
        osg::ref_ptr<osgShadow::ShadowedScene> world = new osgShadow::ShadowedScene;
        world->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
        world->setCastsShadowTraversalMask(CastsShadowTraversalMask);
    
        osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;  
        int mapresx = 1024, mapresy=1024;
        sm->setTextureSize(osg::Vec2s(mapresx,mapresy));
        sm->setPolygonOffset(osg::Vec2(-2, -2));
        
        world->setShadowTechnique(sm);
#endif
        Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), lightPosition, Vec4(0.95, 0.95, 1., 1.));
        world->addChild(lightBlue);
        if (shadowee)
        {
            world->addChild(shadowee);
            machine->setNodeMask(CastsShadowTraversalMask);
        }
        if (shadowed)
        {
            world->addChild(shadowed);
            shadowed->setNodeMask(ReceivesShadowTraversalMask);
        }
        return world;
    }

    
    osg::ref_ptr<osg::Geode> createBase(const osg::Vec3f& center,float radius)
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
        
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(geom);
        
        return geode;
    }
    
    bool LibCogmatix::loadShaderSource(osg::Shader* obj, const std::string& fileName )
    {
        std::string fqFileName = osgDB::findDataFile(fileName);
        if( fileName.length() == 0 )
        {
            std::cout << "File \"" << fileName << "\" not found." << std::endl;
            return false;
        }
        bool success = obj->loadShaderSourceFromFile( fileName.c_str());
        if ( !success  )
        {
            std::cout << "Couldn't load file: " << fileName << std::endl;
            return false;
        }
        else
        {
            return true;
        }
    }

    
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