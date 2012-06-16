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

#include <iostream>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgWidget/WindowManager>
#include <osgGA/StateSetManipulator>

#include "OsgPlugins.h"
#include "LibCogmatix/Machine.h"
#include "LibCogmatix/Clock.h"
#include "LibCogmatix/EventHandler.h"

using namespace LibCogmatix;

int main( int argc, char **argv )
{    
    const unsigned int MASK_2D = 0xF0000000;
	osgViewer::Viewer* _viewer = new osgViewer::Viewer();
    
    osgDB::setLibraryFilePathList("."); 
    
	osgWidget::WindowManager* wm = new osgWidget::WindowManager(
		_viewer,
		1024.0f,
		768.0f,
		MASK_2D,
		0 //osgWidget::WindowManager::WM_PICK_DEBUG
		);
	osg::Camera* camera = wm->createParentOrthoCamera();
	_viewer->getCamera()->setClearColor(osg::Vec4(0.0,0.0,0.0,1.0));
    
    //osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
    //hints->setDetailRatio(2.0f);

    osg::ref_ptr<osg::Group> root = new osg::Group;
   	root->addChild(camera);     
    LibCogmatix::Machine::Ptr machine = createTestMachine(root);
    osg::ref_ptr<LibCogmatix::EventHandler> handler = new LibCogmatix::EventHandler(_viewer, wm, machine);
	_viewer->setSceneData(root);

    osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
	unsigned int clearMask = _viewer->getCamera()->getClearMask();
	_viewer->getCamera()->setClearMask(clearMask | GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	_viewer->getCamera()->setClearStencil(0);

    _viewer->addEventHandler(new osgWidget::MouseHandler(wm));
    _viewer->addEventHandler(new osgWidget::KeyboardHandler(wm));
    _viewer->addEventHandler(new osgWidget::ResizeHandler(wm, camera));
    _viewer->addEventHandler(new osgWidget::CameraSwitchHandler(wm, camera));
    _viewer->addEventHandler(new osgViewer::StatsHandler());
    _viewer->addEventHandler(new osgViewer::WindowSizeHandler());
    _viewer->addEventHandler(new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet()));
    _viewer->addEventHandler(handler);
    
    _viewer->setUpViewInWindow(
        0,
        0,
        static_cast<int>(wm->getWidth()),
        static_cast<int>(wm->getHeight())
    );
	wm->resizeAllWindows();

	_viewer->realize();

    // testing
	while (!_viewer->done())
	{
       	_viewer->getCamera()->setClearStencil(0);
        handler->performActions();
    
		Clock::get()->tick();
        _viewer->frame(Clock::get()->elapsed());
        //        handler->snapToLimit();
	}

	return 0;
}
