#include "stdafx.h"
#include "PickHandler.h"

#include <osg/io_utils>
#include <osg/Transform>

using namespace Cogmatix;

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
            if (view) pick(view,ea);
            return false;
        }    
        default:
            return false;
    }
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    float x = ea.getX();
    float y = ea.getY();

    if (view->computeIntersections(x,y,intersections))
    {
		clearSelection(view);
		if (!intersections.empty())
		{
			const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());
			foreach (osg::Node* node, intersection.nodePath)
			{
				if (dynamic_cast<ParametricSpurGearPart*>(node))
					toggleSelection(view, node, intersection.nodePath.front());
			}
		}
    }
}

void PickHandler::toggleSelection(osgViewer::View* view, osg::Node* node, osg::Node* root)
{
	
	addToSelection(view, node, root);
}

void PickHandler::addToSelection(osgViewer::View* view, osg::Node* node, osg::Node* root)
{
	const ParametricSpurGearPart* gear = dynamic_cast<const ParametricSpurGearPart*>(node);
	assert(view && view->getCamera());
	if (gear)
	{
		osg::Camera* camera = view->getCamera();
		Matrix matProj = camera->getProjectionMatrix();
		Matrix matView = camera->getViewMatrix();
		Matrix matWin = camera->getViewport()->computeWindowMatrix();
		Matrix matWorldToScreen = matView * matProj * matWin;

		osg::BoundingSphere sphereWorld = node->getParent(0)->getBound();
		osg::BoundingSphere sphereScreen = LibCogmatix::transform(matWorldToScreen, sphereWorld);

		//osg::BoundingBox boxWorld = gear->getBoundingBox();
//		osg::BoundingBox boxScreen = boxWorld;
		//osg::BoundingBox boxScreen = osg::transform(matWorldToScreen, boxWorld);

		double width = sphereScreen.radius();
		double height = width;
		double xmin = sphereScreen.center()[0] - width;
		double ymin = sphereScreen.center()[1] - height;

		osg::ref_ptr<osgWidget::Frame> frame = osgWidget::Frame::createSimpleFrameFromTheme(
			"frameTheme",
			osgDB::readImageFile("theme-2.png"),
			width,
			height,
			osgWidget::Frame::FRAME_ALL
			);
		frame->setPosition(xmin, ymin,0);
		frame->getBackground()->setColor(1.0f, 1.0f, 1.0f, 0.0f);
		_wm->addChild(frame);
		_selection[node] = frame;
	}
	
}

void PickHandler::clearSelection(osgViewer::View* view)
{
	foreach (SelectionMap::value_type item, _selection)
	{
		item.second->hide();
	}
	_selection.clear();
}
