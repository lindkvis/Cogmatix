/* OpenSceneGraph example, osgcamera.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "stdafx.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/AnimationPathManipulator>
#include <iostream>

#include <boost/archive/xml_wiarchive.hpp>
#include <boost/archive/xml_woarchive.hpp>

#include "LibCogmatics/Factory.h"


class ModelHandler : public osgGA::GUIEventHandler 
{
public: 

    ModelHandler():
        _position(0) {}
    
    typedef std::vector<std::string> Filenames;
    Filenames _filenames;
    unsigned int _position;
    
    void add(const std::string& filename) { _filenames.push_back(filename); }
        
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        if (!viewer) return false;
    
        if (_filenames.empty()) return false;
    
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYUP):
            {
                if (ea.getKey()=='l')
                {                    
                    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile( _filenames[_position] );
                    ++_position;
                    if (_position>=_filenames.size()) _position = 0;
                    
                    if (model.valid())
                    {
                        viewer->setSceneData(model.get());
                    }
                    
                    return true;
                }
            }
            default: break;
        }
        
        return false;
    }
    
    bool _done;
};


void singleWindowMultipleCameras(osgViewer::Viewer& viewer)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    
    unsigned int width, height;
    wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = width;
    traits->height = height;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (gc.valid())
    {
        osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

        // need to ensure that the window is cleared make sure that the complete window is set the correct colour
        // rather than just the parts of the window that are under the camera's viewports
        gc->setClearColor(osg::Vec4f(0.2f,0.2f,0.6f,1.0f));
        gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
        osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
    }

    unsigned int numCameras = 2;
    double aspectRatioScale = 1.0;///(double)numCameras;
    for(unsigned int i=0; i<numCameras;++i)
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setGraphicsContext(gc.get());
        camera->setViewport(new osg::Viewport((i*width)/numCameras,(i*height)/numCameras, width/numCameras, height/numCameras));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);

        viewer.addSlave(camera.get(), osg::Matrixd(), osg::Matrixd::scale(aspectRatioScale,1.0,1.0));
    }
}

void multipleWindowMultipleCameras(osgViewer::Viewer& viewer, bool multipleScreens)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    
    unsigned int width, height;
    wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);


    unsigned int numCameras = 6;
    double aspectRatioScale = (double)numCameras;
    double translate_x = double(numCameras)-1;
    for(unsigned int i=0; i<numCameras;++i, translate_x -= 2.0)
    {
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->screenNum = multipleScreens ? i / 3 : 0;
        traits->x = (i*width)/numCameras;
        traits->y = 0;
        traits->width = width/numCameras-1;
        traits->height = height;
        traits->windowDecoration = true;
        traits->doubleBuffer = true;
        traits->sharedContext = 0;

        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        if (gc.valid())
        {
            osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;
        }
        else
        {
            osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
        }

        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setGraphicsContext(gc.get());
        camera->setViewport(new osg::Viewport(0,0, width/numCameras, height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);

        viewer.addSlave(camera.get(), osg::Matrix::scale(aspectRatioScale, 1.0, 1.0)*osg::Matrix::translate(translate_x, 0.0, 0.0), osg::Matrix() );
    }
}

class EnableVBOVisitor : public osg::NodeVisitor
{
public:
    EnableVBOVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

    void apply(osg::Geode& geode)
    {
        for(unsigned int i=0; i<geode.getNumDrawables();++i)
        {
            osg::Geometry* geom = geode.getDrawable(i)->asGeometry();
            if (geom)
            {
                osg::notify(osg::NOTICE)<<"Enabling VBO"<<std::endl;
                geom->setUseVertexBufferObjects(true);
            }
        }
    }
};

using namespace LibCogmatics;

int main( int argc, char **argv )
{
	
	osgViewer::Viewer viewer;

	Machine::Ptr machine = Factory::Get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axis = Factory::Get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	Part::Ptr part = Factory::Get()->CreatePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	bool worked = machine->addChild(axis);
	axis->addChild(part);

	viewer.setSceneData(machine.get());
    viewer.run();
    return 0;
}