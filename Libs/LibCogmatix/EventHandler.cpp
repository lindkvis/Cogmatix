#include "stdafx.h"
#include "EventHandler.h"

#include <osg/io_utils>
#include <osg/Transform>
#include <osgFX/Outline>
#include <osg/Material>
#include <osg/BlendFunc>

#include <algorithm>
#include "ActionButton.h"
#include "Action.h"

using namespace LibCogmatix;

bool EventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* o, osg::NodeVisitor* n)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (!view)
        return false;
    
    Vec newPosition = Vec(ea.getX(), ea.getY(), 0.);
    
    float dx = float(ea.getX() - _mx) / ea.getXmax();
    float dy = float(ea.getY() - _my) / ea.getYmax(); 
    
    _mx = ea.getX();
    _my = ea.getY();
    switch(ea.getEventType())
    { 
        case(osgGA::GUIEventAdapter::PUSH):
        {
            if (_selection.valid() && pick(view, ea, false) == _selection.get())
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
                moveSelection (newPosition);
                _dragging=Dragging;
                return true;
            }	
            break;
        }
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            
            if (_dragging != Dragging && (fabs(dx) < 5.0e-1 && fabs(dy) < 5.0e-1))
            {
                return pick(view,ea, true) != nullptr;
            }
            else if (_dragging == Dragging && _selection.valid() && pick(view, ea, false) == _selection.get())
            {
                LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(_selection.get());
                if (mnode)
                    mnode->snapTo();
                _dragging = NotDragging;
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

    osgWidget::WidgetList widgets;
    if (bSelect && _wm->pickAtXY(x, y, widgets))
    {
        LibCogmatix::ActionButton* button = dynamic_cast<LibCogmatix::ActionButton*>(widgets.front().get());
        if (button)
        {
            queueAction(button->action());            
            return nullptr;
        }
    }
    
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
    osgFX::Outline* parentAsOutline = dynamic_cast<osgFX::Outline*>(parent);
    if (!parentAsOutline)
    {
        addToSelection(node);
    }
    else
    {
        removeFromSelection(node);
    }
}

void removeLabels(osgWidget::Window* window)
{
    while (window->begin() != window->end()) {
        window->removeWidget(window->begin()->get());
    }
    int size = window->getObjects().size();
    int size2 = window->getNumObjects();
    window->getObjects().clear();
}

void createLabels(EventHandler* handler, osgWidget::Window* window, const osg::observer_ptr<osg::Node> selection)
{
    const LibCogmatix::MachineNode* node = dynamic_cast<LibCogmatix::MachineNode*>(selection.get());
    if (!node)
        return;
        
    LibCogmatix::Actions nodeActions = node->validActions();
    foreach(const Action& action, nodeActions)
    {
        osg::ref_ptr<ActionButton> button = new ActionButton(handler, action.name);
        window->addWidget(button);        
    }
}
                  
void EventHandler::addToSelection(osg::Node* node)
{
    removeFromSelection(_selection);
    if (node && node->getNumParents() == 1)
    {
        osg::Group* parent = dynamic_cast<osg::Group*>(node->getParent(0));
        if (parent)
        {
            // node not already picked, so highlight it with an osgFX::Outline
            osg::ref_ptr<osgFX::Outline> Outline = new osgFX::Outline();
            Outline->setWidth(6.);
            Outline->setColor(Vec4(1.0, 1.0, 0.0, 0.5)); 
            Outline->addChild(node);
            parent->replaceChild(node,Outline);
            _selection = node;
            
            // Create StateSet and Material
            osg::StateSet* state = node->getOrCreateStateSet(); 
            // Turn on blending 
            osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, 
                                                    osg::BlendFunc::ONE_MINUS_SRC_ALPHA ); 
            state->setAttributeAndModes(bf);
            state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        }
    }

    removeLabels(_labelWindow);
    createLabels(this, _labelWindow, _selection);
}

void EventHandler::removeFromSelection(osg::observer_ptr<osg::Node> selection)
{
    osg::Node* node = dynamic_cast<osg::Node*>(selection.get());
    if (node && node->getNumParents() == 1)
    {
        osgFX::Outline* parentAsOutline = dynamic_cast<osgFX::Outline*>(node->getParent(0));
        if (parentAsOutline)
        {
            // node already picked so we want to remove Outline to unpick it.
            osg::Node::ParentList parentList = parentAsOutline->getParents();
            for(osg::Node::ParentList::iterator itr=parentList.begin();
                itr!=parentList.end();
                ++itr)
            {
                (*itr)->replaceChild(parentAsOutline,node);
            }
        }
        osg::StateSet* state = node->getOrCreateStateSet(); 
        state->clear();

        _selection = nullptr;
    }
    
    removeLabels(_labelWindow);
    createLabels(this, _labelWindow, _selection);
}

void EventHandler::moveSelection(Vec screenPos)
{
    if (_selection.valid())
    {
        LibCogmatix::MachineNode* mnode = dynamic_cast<LibCogmatix::MachineNode*>(_selection.get());
        if (mnode)
        {
            osg::Camera* camera = _viewer->getCamera();

            osg::Matrix gearMatrix = mnode->worldMatrix();

            osg::Matrix viewMatrix = camera->getViewMatrix();
            osg::Matrix projMatrix = camera->getProjectionMatrix();
            osg::Matrix winMatrix =  camera->getViewport()->computeWindowMatrix();
            
            osg::Matrix MW2V (viewMatrix * projMatrix * winMatrix);
            osg::Matrix MV2W = osg::Matrix::inverse(MW2V);
            
            Vec oldScreenPos = mnode->origin() * gearMatrix * MW2V;
            Vec newScreenPos = Vec (screenPos[0], screenPos[1], oldScreenPos[2]);
            
            osg::Matrixd gearMatrixInv = osg::Matrixd::inverse(gearMatrix);
            mnode->setOrigin (newScreenPos * MV2W * gearMatrixInv);
        }
    }
}

void EventHandler::queueAction(CoString action)
{
    dispatchAction(ActionPair(action, _selection));
    _bClean = true;
}

void EventHandler::performActions()
{
    //        toggleSelection(_selection);
    // XXX don't do this, we're removing a label that is in the middle of dispatching an event
    if (_bClean)
    {
        _labelWindow->hide();
        removeLabels(_labelWindow);
        createLabels(this, _labelWindow, _selection);
        _labelWindow->show();
        _bClean=false;
    }
}

void EventHandler::dispatchAction(ActionPair action)
{
    LibCogmatix::MachineNode* node = dynamic_cast<LibCogmatix::MachineNode*>(action.second.get());
    if (!node)
        return;
    
    ActionArgs args;
    node->perform(action.first, args);
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
