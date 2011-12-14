#include "stdafx.h"
#include "PickHandler.h"

#include <osg/io_utils>

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
		std::for_each(allof(intersections), [&](const osgUtil::LineSegmentIntersector::Intersection& intersection)
		{
			if (intersection.nodePath.size() > 2 && _machine)
				toggleSelection(view, *(intersection.nodePath.end()-2));
		});
    }
}

void PickHandler::toggleSelection(osgViewer::View* view, osg::Node* node)
{
	clearSelection(view);
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
		Matrix matScreenToWorld = matView * matProj * matWin;
		Matrix matWorldToScreen = Matrix::inverse(matScreenToWorld);
		osg::BoundingBox boxWorld = gear->getBoundingBox();

		Vec c1 = boxWorld._min();
		Vec c2 = boxWorld._max();

		Vec center = boxWorld._min() * matScreenToWorld;
		double radius = boxWorld.radius();

		double width = 100.;
		double height = 100.;
		osgWidget::Frame* frame = osgWidget::Frame::createSimpleFrameFromTheme(
			"frameTheme",
			osgDB::readImageFile("theme-2.png"),
			width,
			height,
			osgWidget::Frame::FRAME_ALL
			);
		frame->setPosition(center[0]-radius,center[1]-radius,0);
		frame->getBackground()->setColor(1.0f, 1.0f, 1.0f, 0.0f);

		_wm->addChild(frame);

		_selection.push_back(node);
		_frames.push_back(frame);
	}
	
}

void PickHandler::clearSelection(osgViewer::View* view)
{
	std::for_each(allof(_selection), [this](osg::Node* selected)
	{
		osg::StateSet* gearState = selected->getOrCreateStateSet();
		//	gearState->removeAttribute(_selectionShader);
	});
	_selection.clear();


}