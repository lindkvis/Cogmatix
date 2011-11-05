#pragma once

#include <osg/MatrixTransform>

#include "Node.h"

namespace LibCogmatics
{
	class Machine : public NamedMachineNode	
	{
	protected:
		std::vector<MachineNode::Ptr> _nodes; // contains pointers to all the nodes in the machine.
	public:
		typedef osg::ref_ptr<Machine> Ptr;
		typedef osg::ref_ptr<const Machine> CPtr;

		Machine(NodeID ID, const String& name);
		~Machine(void);
	};
}
