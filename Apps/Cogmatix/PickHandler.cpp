#include "stdafx.h"
#include "PickHandler.h"

#include <osg/io_utils>
#include <osg/Transform>
#include <osgFX/Scribe>

using namespace Cogmatix;

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        case(osgGA::GUIEventAdapter::MOVE):
        {
            _mx = ea.getX();
            _my = ea.getY();
            return false;
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            if (fabs(_mx - ea.getX()) < 1.0e-2 && fabs(_my - ea.getY()) - 1.0e-2)
            {
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                if (view) pick(view,ea);
                return false;
            }
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
	//	clearSelection(view);
		if (!intersections.empty())
		{
			const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());
            const osg::NodePath& nodePath = intersection.nodePath;
            osg::Node* node = (nodePath.size()>=2)?nodePath[nodePath.size()-2]:0;
            osg::Group* parent = (nodePath.size()>=3)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-3]):0;
            toggleSelection(view, node, parent);
		}
    }
}

void PickHandler::toggleSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent)
{
    osgFX::Scribe* parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent);
    if (!parentAsScribe)
    {
        // node not already picked, so highlight it with an osgFX::Scribe
        osgFX::Scribe* scribe = new osgFX::Scribe();
        scribe->addChild(node);
        parent->replaceChild(node,scribe);
    }
    else
    {
        // node already picked so we want to remove scribe to unpick it.
        osg::Node::ParentList parentList = parentAsScribe->getParents();
        for(osg::Node::ParentList::iterator itr=parentList.begin();
            itr!=parentList.end();
            ++itr)
        {
            (*itr)->replaceChild(parentAsScribe,node);
        }
    }
}

