#include "stdafx.h"
#include "EventHandler.h"

#include <osg/io_utils>
#include <osg/Transform>
#include <osgFX/Scribe>

#include <osgWidget/Label>

#include "LibCogmatix/Action.h"

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
            if ((fabs(dx) > 1.0e-2 && fabs(dy) > 1.0e-2) && (_dragging == DragInitiated || _dragging == Dragging))
            {
                moveSelection (newPosition - _oldPosition);
                _dragging=Dragging;
            }
            break;
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            if (_dragging != Dragging && fabs(dx) < 1.0e-2 && fabs(dy) < 1.0e-2)
            {
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                if (view) pick(view,ea);
            }
            else if (_dragging == Dragging)
            {
                if (!_selection.empty())
                {
                    LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(_selection.front());
                    if (mnode)
                        mnode->snapTo();
                }
                // _viewer->setCameraManipulator(_cameraManipulator); 
            }
            _dragging = NotDragging;
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

    osg::ref_ptr<osgUtil::LineSegmentIntersector> ray = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION, ea.getXnormalized(), ea.getYnormalized());
    osgUtil::IntersectionVisitor visitor(ray);
    _viewer->getCamera()->accept(visitor);

    if (ray->containsIntersections())
    {
        const osgUtil::LineSegmentIntersector::Intersection& intersection = ray->getFirstIntersection();
        const osg::NodePath& nodePath = intersection.nodePath;
        osg::Node* node = (nodePath.size()>=2)?nodePath[nodePath.size()-2] : nullptr;
        osg::Group* parent = (nodePath.size()>=3)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-3]) : nullptr;
        toggleSelection(view, node, parent);
    }
    else 
    {
        foreach(osg::Node* sel, _selection)
        {
            toggleSelection(view, sel, sel->getParent(0));  
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
        _selection.push_back(node);
        addedToSelection(node);
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
        std::list<osg::Node*>::iterator it = std::find(allof(_selection), node);
        if (it != _selection.end()) {
            _selection.erase(it);
            removedFromSelection(node);
        }
    }
}

void removeLabels(osgWidget::Window* window)
{
    while (window->begin() != window->end()) {
        window->removeWidget(window->begin()->get());
    }
}

void createLabels(osgWidget::Window* window, std::list<osg::Node*> selection)
{
    std::set<Action> actions;
    
    for (std::list<osg::Node*>::iterator itr = selection.begin();
         itr != selection.end();
         ++itr)
    {
        LibCogmatix::MachineNode* node = dynamic_cast<LibCogmatix::MachineNode*>(*itr);
        if (!node)
            continue;
        
        LibCogmatix::Actions nodeActions = node->validActions();
        actions.insert(nodeActions.begin(), nodeActions.end());
    }
    
    for (std::set<Action>::iterator itr = actions.begin();
         itr != actions.end();
         ++itr)
    {
        LibCogmatix::Action action = *itr;
        osgWidget::Label* label = new osgWidget::Label("", action.name);
        label->setFont("fonts/Vera.ttf");
        label->setFontSize(30);
        label->setColor(0.8f, 0.2f, 0.2f, 0.8f);
        label->setCanFill(true);
        label->setShadow(0.1f);
        label->addSize(20.0f, 20.0f);
        window->addWidget(label);
    }
}
                  
void EventHandler::addedToSelection(osg::Node* node)
{
    removeLabels(_labelWindow);
    createLabels(_labelWindow, _selection);
}

void EventHandler::removedFromSelection(osg::Node* node)
{
    removeLabels(_labelWindow);
    createLabels(_labelWindow, _selection);
}

void EventHandler::moveSelection(Vec worldShift)
{
    foreach (osg::Node* node, _selection)
    {
        LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(node);
        if (mnode)
        {
            double axialMove = mnode->worldAxis() * worldShift;
            Vec planeShift = worldShift - mnode->worldAxis() * axialMove;
            osg::Matrixd gearMatrix = mnode->worldMatrix();
            Vec origin = mnode->origin() * gearMatrix;
            origin += planeShift;
            osg::Matrixd gearMatrixInv = osg::Matrixd::inverse(gearMatrix);
            mnode->setOrigin (origin * gearMatrixInv);
        }
    }
}

