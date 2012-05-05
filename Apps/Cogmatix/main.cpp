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
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <iostream>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osgWidget/Util>
#include <osgWidget/WindowManager>
#include <osg/Material>
#include <osgGA/StateSetManipulator>

#include "OsgPlugins.h"
#include "LibCogmatix/Machine.h"
#include "LibCogmatix/Clock.h"
#include "EventHandler.h"



using namespace LibCogmatix;
using namespace Cogmatix;

bool loadShaderSource(osg::Shader* obj, const std::string& fileName )
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

const unsigned int MASK_2D = 0xF0000000;

int main( int argc, char **argv )
{    
	osgViewer::Viewer viewer;
    osgDB::setLibraryFilePathList("."); 
    
	osgWidget::WindowManager* wm = new osgWidget::WindowManager(
		&viewer,
		1024.0f,
		768.0f,
		MASK_2D,
		0 //osgWidget::WindowManager::WM_PICK_DEBUG
		);
	osg::Camera* camera = wm->createParentOrthoCamera();
	viewer.getCamera()->setClearColor(osg::Vec4(0.0,0.0,0.0,1.0));
    
    //osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
    //hints->setDetailRatio(2.0f);

    osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(camera);        
    LibCogmatix::Machine::Ptr machine = createTestMachine(root);
    osg::ref_ptr<EventHandler> handler = new EventHandler(&viewer, wm, machine);
	viewer.setSceneData(root);

	unsigned int clearMask = viewer.getCamera()->getClearMask();
	viewer.getCamera()->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	viewer.getCamera()->setClearStencil(0);

    viewer.addEventHandler(new osgWidget::MouseHandler(wm));
    viewer.addEventHandler(new osgWidget::KeyboardHandler(wm));
    viewer.addEventHandler(new osgWidget::ResizeHandler(wm, camera));
    viewer.addEventHandler(new osgWidget::CameraSwitchHandler(wm, camera));
    viewer.addEventHandler(new osgViewer::StatsHandler());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler());
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.addEventHandler(handler);
    
    viewer.setUpViewInWindow(
        50,
        50,
        static_cast<int>(wm->getWidth()),
        static_cast<int>(wm->getHeight())
    );
	wm->resizeAllWindows();

	viewer.realize();
	//Clock::get()->start();

    // testing
    bool addedNewGear=false;
    bool bStopped=false;
    bool bStarted=false;
	while (!viewer.done())
	{
		Clock::get()->tick();
        viewer.frame(Clock::get()->elapsed());
        handler->snapToLimit();
        /*
        if (Clock::get()->elapsed() > 10.)
            gear4->perform("remove", ActionArgs());
        if (Clock::get()->elapsed() > 5.2 && !addedNewGear)
        {
            gear5->perform("copy", ActionArgs());
            addedNewGear=true;
        }        
        if (Clock::get()->elapsed() > 2.5 && !bStopped)
        {
            motor->perform("stop", ActionArgs());
            bStopped=true;
        }
        if (Clock::get()->elapsed() > 7.5 && !bStarted)
        {
            motor->perform("start", ActionArgs());
            bStarted=true;
        }*/
            
	}


	return 0;
}
