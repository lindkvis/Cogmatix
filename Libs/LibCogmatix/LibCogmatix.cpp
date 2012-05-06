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
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

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
        BoxMotor::Ptr motor = Factory::get()->CreateBoxMotor(20, Vec(0., 1., 0.), Vec(0., 5., 0.), Vec(5., 5., 5.), 1.0, 7.5);
        machine->addChild(motor);
        
        ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(0., 0., 0.), 40, 1.5, 1.0, 0.3, 0.);
        ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(7.5, 0., 0.),10, 1.0, 0.5, 0.3, 0.);
        ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 0.), 10, 1.0, 0.5, 0.3, 0.);
        ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 5.1), 24, 1.0, 0.5, 0.3, 0.);
        ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0, 11.7), 20, 1.0, 0.5, 0.3, 0.);
        
        //ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, -2., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
        //ParametricSpurGearPart::Ptr gear6 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 0., 1.), Vec(10.5, -6., 10.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
        machine->addChild(gear);
        machine->addChild(gear2);
        machine->addChild(gear3);
        machine->addChild(gear4);
        machine->addChild(gear5);
        osg::ref_ptr<osg::Node> plane_geode = createBase(Vec(0., 8., 0.), 100);
        plane_geode->setNodeMask(ReceivesShadowTraversalMask);
        osg::StateSet* planeState = plane_geode->getOrCreateStateSet();
        
        osg::ref_ptr<osg::Texture2D> planeTex = new osg::Texture2D(osgDB::readImageFile("paper_tile.jpg"));
        //    planeTex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
        //    planeTex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
        planeTex->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);    
        planeTex->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);    
        planeState->setTextureAttributeAndModes( 0, planeTex , osg::StateAttribute::ON);    
        //machine->addChild(gear6);
        Clock::get()->add(motor);
        
        osg::StateSet* gearState = machine->getOrCreateStateSet();
        motor->start();
        
        osg::Group* world = createShadowedScene(machine, plane_geode, Vec(30., -200., 30.), 10., 0);
        //osg::ref_ptr<osg::Group> world = new osg::Group;
      /*  osg::ref_ptr<osgShadow::ShadowedScene> world = new osgShadow::ShadowedScene;
        world->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
        world->setCastsShadowTraversalMask(CastsShadowTraversalMask);
        
        //#ifdef TARGET_OS_IPHONE
        //        osg::ref_ptr<osgShadow::ShadowTexture> sm = new osgShadow::ShadowTexture;
        //#else
        osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;  
        //#endif
        int mapresx = 1024, mapresy=1024;
        sm->setTextureSize(osg::Vec2s(mapresx,mapresy));
        sm->setPolygonOffset(osg::Vec2(-2, -2));
        
       //#endif
        world->setShadowTechnique(sm);    
        
       
        root->addChild(world);
        //        world->addChild(machine);
        //        world->addChild(plane_geode);
        */
        //   Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(30., -200., 30.), Vec4(0.95, 0.95, 1., 1.));
        //        world->addChild(lightBlue);
        return machine;
    }
    
    osg::Group* createShadowedScene(osg::Node* shadower,osg::Node* shadowed,const osg::Vec3& lightPosition,float radius,unsigned int unit)
    {
        osg::Group* group = new osg::Group;
        
        // add light source
        {
            osg::LightSource* lightSource = new osg::LightSource;
            lightSource->getLight()->setPosition(osg::Vec4(lightPosition,1.0f));
            lightSource->getLight()->setLightNum(0);
            
            group->addChild(lightSource);
            
            osg::Geode* lightgeode = new osg::Geode;
            lightgeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            lightgeode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(lightPosition,radius)));
            group->addChild(lightgeode);
        }
        
        osg::Vec4 ambientLightColor(0.2,0.2f,0.2f,1.0f);
        
        // add the shadower and shadowed.
        group->addChild(shadower);
        group->addChild(shadowed);
        
        
        unsigned int tex_width = 512;
        unsigned int tex_height = 512;
        
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setTextureSize(tex_width, tex_height);
        texture->setInternalFormat(GL_RGB);
        texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
        texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
        texture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
        
        // set up the render to texture camera.
        {
            
            // create the camera
            osg::Camera* camera = new osg::Camera;
            
            camera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
            
            // set viewport
            camera->setViewport(0,0,tex_width,tex_height);
            
            // set the camera to render before the main camera.
            camera->setRenderOrder(osg::Camera::PRE_RENDER);
            
            // tell the camera to use OpenGL frame buffer object where supported.
            camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
            
            // attach the texture and use it as the color buffer.
            camera->attach(osg::Camera::COLOR_BUFFER, texture);
            
            // add subgraph to render
            camera->addChild(shadower);
            
            osg::StateSet* stateset = camera->getOrCreateStateSet();
            
            // make the material black for a shadow.
            osg::Material* material = new osg::Material;
            material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,0.0f,1.0f));
            material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,0.0f,1.0f));
            material->setEmission(osg::Material::FRONT_AND_BACK,ambientLightColor);
            material->setShininess(osg::Material::FRONT_AND_BACK,0.0f);
            stateset->setAttribute(material,osg::StateAttribute::OVERRIDE);
            
            group->addChild(camera);
            
            // create the texgen node to project the tex coords onto the subgraph
            osg::TexGenNode* texgenNode = new osg::TexGenNode;
            texgenNode->setTextureUnit(unit);
            group->addChild(texgenNode);
            
            // set an update callback to keep moving the camera and tex gen in the right direction.
            //            group->setUpdateCallback(new UpdateCameraAndTexGenCallback(lightPosition, camera, texgenNode));
        }
        
        // set the shadowed subgraph so that it uses the texture and tex gen settings.    
        {
            osg::StateSet* stateset = shadowed->getOrCreateStateSet();
            stateset->setTextureAttributeAndModes(unit,texture,osg::StateAttribute::ON);
            stateset->setTextureMode(unit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
            stateset->setTextureMode(unit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
            stateset->setTextureMode(unit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
            stateset->setTextureMode(unit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON);
        }
        
        
        // set hud to render shadow texture, just for interest
        {
            osg::Geode* geode = new osg::Geode;
            osg::Geometry* geom = osg::createTexturedQuadGeometry(osg::Vec3(0,0,0),osg::Vec3(100.0,0.0,0.0),osg::Vec3(0.0,100.0,0.0));
            geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
            geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            geode->addDrawable(geom);
            
            osg::Camera* camera = new osg::Camera;
            
            // set the projection matrix
            camera->setProjectionMatrix(osg::Matrix::ortho2D(0,100,0,100));
            
            // set the view matrix    
            camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
            camera->setViewMatrix(osg::Matrix::identity());
            
            camera->setViewport(50,50,100,100);
            
            // only clear the depth buffer
            camera->setClearMask(GL_DEPTH_BUFFER_BIT);
            
            // draw subgraph after main camera view.
            camera->setRenderOrder(osg::Camera::POST_RENDER);
            
            camera->addChild(geode);
            
            group->addChild(camera);
            
        }
        
        return group;
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