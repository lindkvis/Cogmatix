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
				_machine->toggleSelection(*(intersection.nodePath.end()-2));
		});
    }
}