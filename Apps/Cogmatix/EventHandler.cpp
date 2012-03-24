#include "stdafx.h"
#include "EventHandler.h"

#include <osg/io_utils>
#include <osg/Transform>
#include <osgFX/Scribe>

using namespace Cogmatix;

bool EventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    osg::Camera* camera = _viewer->getCamera();
    osg::Matrix viewMatrix = camera->getViewMatrix();
    osg::Matrix projMatrix = camera->getProjectionMatrix();
    osg::Matrix winMatrix =  camera->getViewport()->computeWindowMatrix();

    osg::Matrix MW2V (viewMatrix * projMatrix * winMatrix);
    osg::Matrix MV2W = osg::Matrix::inverse(MW2V);
    Vec newPosition = Vec(ea.getX(), ea.getY(), 0.) * MV2W;
    
    float dx = ea.getX() - _mx;
    float dy = ea.getY() - _my;
    _mx = ea.getX();
    _my = ea.getY();
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
            if (!_selection.empty())
            {
                _dragging=DragInitiated;
                _viewer->setCameraManipulator(nullptr); 
                return true;
            }
            break;
        }
        case(osgGA::GUIEventAdapter::DRAG):
        { 
            if (_dragging == DragInitiated || _dragging == Dragging) 
            {
                moveSelection (newPosition - _oldPosition);
                _dragging=Dragging;
            }
            break;
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            if (_dragging != Dragging && fabs(dx) < 1.0e-1 && fabs(dy) < 1.0e-1)
            {
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                if (view) pick(view,ea);
            }
            if (_dragging == Dragging)
                _dragging = NotDragging;
            //_dragging=NotDragging;
            //_viewer->setCameraManipulator(_cameraManipulator); 
            break;
        }
        default:
            break;
    }
    _oldPosition = newPosition;
    return false;
}

void EventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
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

void EventHandler::toggleSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent)
{
    osgFX::Scribe* parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent);
    if (!parentAsScribe)
    {
        // node not already picked, so highlight it with an osgFX::Scribe
        osgFX::Scribe* scribe = new osgFX::Scribe();
        scribe->addChild(node);
        parent->replaceChild(node,scribe);
        _selection.insert(node);
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
        _selection.erase(node);
    }
}

void EventHandler::moveSelection(Vec worldShift)
{
       foreach (osg::Node* node, _selection)
    {
        LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(node);
        if (mnode)
        {
            osg::Matrixd gearMatrix = mnode->worldMatrix();
            Vec origin = mnode->origin() * gearMatrix;
            origin += worldShift;
            osg::Matrixd gearMatrixInv = osg::Matrixd::inverse(gearMatrix);
            mnode->setOrigin (origin * gearMatrixInv);
        }
    }
}

