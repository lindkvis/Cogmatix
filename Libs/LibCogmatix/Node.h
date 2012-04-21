#pragma once

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <fstream>
#include <assert.h>

#include "LibCogmatix.h"
#include "Action.h"

namespace LibCogmatix
{
    // Recursive find function
    template<class ClassType>
    const ClassType* findChildOfType (const osg::Node* node)
    {
        const ClassType* found = dynamic_cast<const ClassType*>(node);
        if (found)
            return found;
        const osg::Group* group = dynamic_cast<const osg::Group*>(node);
        for (int i=0; group && i < group->getNumChildren(); ++i)
        {
            const ClassType* child = findChildOfType<ClassType>(group->getChild(i));
            if (child)
                return child;
        }
        const osg::Geode* geode = dynamic_cast<const osg::Geode*>(node);
        for (int i=0; geode && i < geode->getNumDrawables(); ++i)
        {
            const ClassType* child = dynamic_cast<const ClassType*>(geode->getDrawable(i));
            if (child)
                return child;
        }
        return nullptr;
    }
    
    // Recursive find function
    template<class ClassType>
    ClassType* findChildOfType (osg::Node* node)
    {
        ClassType* found = dynamic_cast<ClassType*>(node);
        if (found)
            return found;
        osg::Group* group = dynamic_cast<osg::Group*>(node);
        for (int i=0; group && i < group->getNumChildren(); ++i)
        {
            ClassType* child = findChildOfType<ClassType>(group->getChild(i));
            if (child)
                return child;
        }
        osg::Geode* geode = dynamic_cast<osg::Geode*>(node);
        for (int i=0; geode && i < geode->getNumDrawables(); ++i)
        {
            ClassType* child = dynamic_cast<ClassType*>(geode->getDrawable(i));
            if (child)
                return child;
        }
        
        return nullptr;
    }	
    
    // Recursive find function
    template<class ClassType>
    void findDescendantsOfType (osg::Node* node, std::list<ClassType*>& matches)
    {
        assert (node);
        ClassType* found = dynamic_cast<ClassType*>(node);
        if (found)
            matches.push_back(found);
        osg::Group* group = dynamic_cast<osg::Group*>(node);
        for (int i=0; group && i < group->getNumChildren(); ++i)
        {
            findDescendantsOfType<ClassType>(group->getChild(i), matches);
        }
    }	
    
    template<class ClassType>
    void findParentsOfType (osg::Node* node, std::list<ClassType*>& matches)
    {
        assert (node && node->getNumParents() <= 1);
        if (!node || node->getNumParents() != 1) return;
        ClassType* match = dynamic_cast<ClassType*>(node->getParent(0));
        if (match)
            matches.push_back(match);
        findParentsOfType<ClassType>(node->getParent(0), matches);
    }
    
    
	/**
	 * Templated Node interface, to inject Cogmatix specific information into the scenegraph.
	 */
	class MachineNode
	{
    public:
        enum Compatibility
        {
            Compatible=0,
            NotDetermined, // Not enough information to determine compatibility
            OnAxis, // driven by gears axis.
            Self,
            AlreadyMoving,
            CanSnapTo,
            TooFarAway,
            DifferentPlane,
            Conflict
        };
        
        NodeID ID() const { return _ID; }
        /**
         * Retrieve a list of valid actions for the given node
         * Any sub-class should specialise this and explicitly call the parent method 
         * to inject the parent actions.
         */
        virtual Actions validActions () const;
        virtual ActionResult perform (CoString action, const ActionArgs& args);
        virtual bool snapTo (const MachineNode* master) { return false; }
        virtual bool snapTo () { return false; }
        virtual void setOrigin (Vec newOrigin) {}
        virtual Vec origin() const { return Vec(0., 0., 0.); }
        
        // Get the world axis
        virtual Vec worldAxis() const { return Vec(0., 0., 0.); }
        // Get the world position
        virtual Vec worldPosition() const { return Vec(0., 0., 0.); }
        // Get the world matrix for the spur gear
        virtual osg::Matrixd worldMatrix() const { return osg::Matrixd(); }
        // Get the world bounding sphere of the spur gear
        virtual osg::BoundingSphere worldBound() const { return osg::BoundingSphere(); }
        
        virtual Compatibility isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const
        {
            return NotDetermined;
        }
        
	protected:
		NodeID _ID; ///<Machine specific ID. Unique within the machine.

		/**
		* Made protected because the constructors should never be called manually.
		*/
        MachineNode(NodeID ID) : _ID (ID) {}
		virtual ~MachineNode() {};
        MachineNode() : _ID(-1) {}
   	factory_protected:
        void setID (NodeID ID) { _ID = ID; }
	};
    
    template<class T>
    class TMachineNode : public MachineNode, public T
    {
        META_Node(LibCogmatix, TMachineNode<T>);
    factory_protected:
        TMachineNode() : MachineNode(), T() {}
        TMachineNode(NodeID ID, CoString name="") : MachineNode(ID) { T::setName (name); }
        TMachineNode(NodeID ID, const TMachineNode<T>& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
            : MachineNode(ID), T(copyFrom, copyop)
        {
        }
        TMachineNode(const TMachineNode<T>& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
            : MachineNode(-1), T(copyFrom, copyop)
        {
        }
        virtual ~TMachineNode() {}
    };
}
