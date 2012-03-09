#pragma once

#include <osg/MatrixTransform>
#include <fstream>

#include "LibCogmatix.h"

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
    
    class RotaryAxis;
	class LinearAxis;

	/**
	 * Abstract Node class. Any Cogmatic machine component will inherit from this
	 */
	class MachineNode
	{
	public:
		NodeID ID() { return _ID; }
	protected:
		NodeID _ID; ///<Machine specific ID. Unique within the machine.

		/**
		* Made protected because the constructors should never be called manually.
		*/
		MachineNode(NodeID ID) : _ID (ID) {}
		virtual ~MachineNode() {};
	private:
	};
}
