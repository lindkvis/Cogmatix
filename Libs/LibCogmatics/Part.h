#pragma once

#include <osg/Geode>

#include "Node.h"

namespace LibCogmatics
{
	class Part : public NamedMachineNode, public osg::Geode
	{
	public:
		typedef osg::ref_ptr<Part> Ptr;
		typedef osg::ref_ptr<const Part> CPtr;

	factory_protected:
		Part(NodeID ID, CoString name);
		~Part(void);
	private:
		friend class boost::serialization::access;
	};
}
