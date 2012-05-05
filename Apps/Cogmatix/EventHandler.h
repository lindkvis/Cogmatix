/***********************
* GUI picking handler *
*/

#pragma once

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/PolygonMode>
#include <iostream>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osgWidget/Util>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgWidget/Label>

#include "LibCogmatix/Machine.h"
#include "LibCogmatix/Units.h"

using namespace LibCogmatix;

namespace Cogmatix
{
	class EventHandler : public osgGA::GUIEventHandler 
	{
	protected:
        enum DragState
        {
            NotDragging=0,
            DragInitiated=1,
            Dragging=2
        };
        DragState _dragging;
        osg::ref_ptr<osgGA::TrackballManipulator> _cameraManipulator;
        osgViewer::Viewer* _viewer;
		osgWidget::WindowManager* _wm;
		Machine::Ptr _machine;
        float _mx, _my;
        Vec _oldPosition;
		void toggleSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent);
        osg::observer_ptr<osg::Node> _selection;
        
        osgWidget::Window* _labelWindow;
        
        void addedToSelection(osg::Node* node);
        void removedFromSelection(osg::Node* node);
	public: 
		EventHandler(osgViewer::Viewer* viewer, osgWidget::WindowManager* wm, Machine::Ptr machine) 
            : _viewer(viewer), _wm(wm), _machine(machine), _mx(0.), _my(0.), _dragging(NotDragging) 
        {
            _cameraManipulator = new osgGA::TrackballManipulator();
            _cameraManipulator->setHomePosition(Vec(0., -100, 0.), Vec(0., 0., 0.), Vec(0., 0., 1.), false);
            _viewer->setCameraManipulator(_cameraManipulator);    
            _labelWindow = new osgWidget::Box("", osgWidget::Box::VERTICAL);
            wm->addChild(_labelWindow);
            
        }
		~EventHandler() {}

		bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		virtual osg::Node* pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool bSelect);
        void moveSelection(Vec shift);
        void dispatchAction(CoString action);
        
        void snapToLimit();
	};
}
