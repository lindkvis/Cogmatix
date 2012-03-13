#pragma once

#include <osg/MatrixTransform>
#include <fstream>

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
        return nullptr;
    }	
    
    // Recursive find function
    template<class ClassType>
    void findDescendantsOfType (osg::Node* node, std::list<ClassType*>& matches)
    {
        ClassType* found = dynamic_cast<ClassType*>(node);
        if (found)
            matches.push_back(found);
        osg::Group* group = dynamic_cast<osg::Group*>(node);
        for (int i=0; group && i < group->getNumChildren(); ++i)
        {
            findDescendantsOfType<ClassType>(group->getChild(i), matches);
        }
    }	
    
	/**
	 * Templated Node interface, to inject Cogmatix specific information into the scenegraph.
	 */
	class MachineNode
	{
    public:
        NodeID ID() const { return _ID; }
        /**
         * Retrieve a list of valid actions for the given node
         * Any sub-class should specialise this and explicitly call the parent method 
         * to inject the parent actions.
         */
        virtual Actions validActions () const;
        virtual ActionResult perform (CoString action, const ActionArgs& args);
        virtual bool snapTo (const MachineNode* master) { return false; }
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
