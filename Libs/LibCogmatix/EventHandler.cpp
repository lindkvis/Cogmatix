#include "stdafx.h"
#include "EventHandler.h"

#include <osg/io_utils>
#include <osg/Transform>
#include <osgFX/Scribe>

#include <algorithm>
#include "ActionButton.h"
#include "LibCogmatix/Action.h"

using namespace LibCogmatix;

bool EventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (!view)
        return false;
    osg::Camera* camera = _viewer->getCamera();
    osg::Matrix viewMatrix = camera->getViewMatrix();
    osg::Matrix projMatrix = camera->getProjectionMatrix();
    osg::Matrix winMatrix =  camera->getViewport()->computeWindowMatrix();

    osg::Matrix MW2V (viewMatrix * projMatrix * winMatrix);
    osg::Matrix MV2W = osg::Matrix::inverse(MW2V);
    Vec newPosition = Vec(ea.getX(), ea.getY(), 0.) * MV2W;
    
    float dx = float(ea.getX() - _mx) / ea.getXmax();
    float dy = float(ea.getY() - _my) / ea.getYmax();
    _mx = ea.getX();
    _my = ea.getY();
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
            if (_selection.valid())
            {
                _dragging=DragInitiated;
                _cameraManipulator->setIgnoreHandledEventsMask(osgGA::GUIEventAdapter::DRAG);
                return true;
            }
            break;
        }
        case(osgGA::GUIEventAdapter::DRAG):
        { 
            if (_selection.valid() && (_dragging == DragInitiated || _dragging == Dragging))
            {
                _cameraManipulator->setIgnoreHandledEventsMask(osgGA::GUIEventAdapter::DRAG);
                if (pick(view, ea, false) == _selection.get())
                {
                    moveSelection (newPosition - _oldPosition);
                    _dragging=Dragging;
                }
                return true;
            }	
            break;
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            
            if (_dragging != Dragging && fabs(dx) < 5.0e-2 && fabs(dy) < 5.0e-2)
            {
                pick(view,ea, true);
                return true;
            }
            else if (_dragging == Dragging && _selection.valid())
            {
                LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(_selection.get());
                if (mnode)
                    mnode->snapTo();
                return true;
            }
            _dragging = NotDragging;
            _cameraManipulator->setIgnoreHandledEventsMask(0);
            break;
        }
        default:
            break;
    }
    _oldPosition = newPosition;
    return false;
}

osg::Node* EventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool bSelect)
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
        if (dynamic_cast<LibCogmatix::MachineNode*>(node))
        {
            if (bSelect)
                toggleSelection(view, node, parent);
            return node;
        }
    }

    // no hit
    if (_selection.valid() && bSelect)
    {
        osg::Node* sel = _selection.get();    
        if (sel && sel->getNumParents() > 0)
            toggleSelection(view, sel, sel->getParent(0));  
    }
    return nullptr;
}

void EventHandler::toggleSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent)
{
    osgFX::Scribe* parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent);
    if (!parentAsScribe)
    {
        // node not already picked, so highlight it with an osgFX::Scribe
        osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
        scribe->addChild(node);
        parent->replaceChild(node,scribe);
        if (_selection.valid()) // remove existing selection
        {
            osg::Node* sel = _selection.get();
            toggleSelection(view, sel, sel->getParent(0));
        }
        _selection = node;
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
        _selection = nullptr;
        removedFromSelection(node);
    }
}

void removeLabels(osgWidget::Window* window)
{
    while (window->begin() != window->end()) {
        window->removeWidget(window->begin()->get());
    }
}

void createLabels(EventHandler* handler, osgWidget::Window* window, const osg::observer_ptr<osg::Node> selection)
{
    if (!selection.valid())
        return;
    
    std::set<Action> actions;
    
    const LibCogmatix::MachineNode* node = dynamic_cast<LibCogmatix::MachineNode*>(selection.get());
    if (!node)
        return;
        
    LibCogmatix::Actions nodeActions = node->validActions();
    actions.insert(nodeActions.begin(), nodeActions.end());
    
    for (std::set<Action>::iterator itr = actions.begin();
         itr != actions.end();
         ++itr)
    {
        LibCogmatix::Action action = *itr;
        ActionButton* button = new ActionButton(handler, action.name);
        window->addWidget(button);
    }
}
                  
void EventHandler::addedToSelection(osg::Node* node)
{
    removeLabels(_labelWindow);
    createLabels(this, _labelWindow, _selection);
}

void EventHandler::removedFromSelection(osg::Node* node)
{
    removeLabels(_labelWindow);
    createLabels(this, _labelWindow, _selection);
}

void EventHandler::moveSelection(Vec worldShift)
{
    if (_selection.valid())
    {
        LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(_selection.get());
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

void EventHandler::dispatchAction(CoString action)
{
    if (!_selection.valid())
        return;

    LibCogmatix::MachineNode* node = dynamic_cast<LibCogmatix::MachineNode*>(_selection.get());
    if (!node)
        return;
    
    ActionArgs args;
    node->perform(action, args);
    
    // XXX don't do this, we're removing a label that is in the middle of dispatching an event
    removeLabels(_labelWindow);
    createLabels(this, _labelWindow, _selection);
}

void EventHandler::snapToLimit()
{
    Vec center = _cameraManipulator->getCenter();
    osg::Quat quat = _cameraManipulator->getRotation();
    double a, b, c;
    Vec i (1., 0., 0.), j(0., 1., 0.), k(0., 0., 1.);
    getEulerFromQuat(quat, a, b, c);
    center[0] = between (-50.f, 50.f, center[0]);
    center[1] = between (-30.f, -10.f, center[1]);
    center[2] = between (-50.f, 50.f, center[2]);
    double distance = between(5., 200., _cameraManipulator->getDistance());
    _cameraManipulator->setDistance(distance);
    _cameraManipulator->setCenter(center);
   
    //a = between(-PI, PI, a);
    //b = between(-PI, PI, b);
    // c = between(-PI, PI, c);
    
    quat = osg::Quat(a, i, b, j, c, k);
    
    //_cameraManipulator->setRotation(quat);
    std::cout << center[0] << std::endl;
}
