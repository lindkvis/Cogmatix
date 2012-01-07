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
					toggleSelection(view, node);
			}
		}
    }
}

void PickHandler::toggleSelection(osgViewer::View* view, osg::Node* node)
{
	
	addToSelection(view, node);
}

void PickHandler::addToSelection(osgViewer::View* view, osg::Node* node)
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

		osg::BoundingBox boxWorld = gear->getBoundingBox();
		osg::BoundingBox boxScreen = boxWorld;
		//osg::BoundingBox boxScreen = osg::transform(matWorldToScreen, boxWorld);

		double width  = boxScreen.xMax() - boxScreen.xMin();
		double height = boxScreen.yMax() - boxScreen.yMin();

		osg::ref_ptr<osgWidget::Frame> frame = osgWidget::Frame::createSimpleFrameFromTheme(
			"frameTheme",
			osgDB::readImageFile("theme-2.png"),
			width,
			height,
			osgWidget::Frame::FRAME_ALL
			);
		double wmwidth = _wm->getWidth();
		double viewwidth = view->getCamera()->getViewport()->width();
		frame->setPosition(boxScreen.xMin(),boxScreen.yMin(),0);
		frame->getBackground()->setColor(1.0f, 1.0f, 1.0f, 0.0f);
		_wm->addChild(frame);
		_selection.push_back(node);
	}
	
}

void PickHandler::clearSelection(osgViewer::View* view)
{
	_wm->removeChildren(0, _wm->getNumChildren());
	_selection.clear();


}
