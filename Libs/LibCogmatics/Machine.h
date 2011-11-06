#pragma once

#include <osg/MatrixTransform>
#include <boost/serialization/vector.hpp>

#include "Node.h"

namespace LibCogmatics
{
	class Machine : public NamedMachineNode, public osg::MatrixTransform	
	{
	protected:
		friend class Factory;
		std::vector<osg::ref_ptr<osg::Node>> _nodes; // contains pointers to all the nodes in the machine.
		Machine(NodeID ID, CoString name);
		~Machine(void);
	public:
		typedef osg::ref_ptr<Machine> Ptr;
		typedef osg::ref_ptr<const Machine> CPtr;
	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(NamedMachineNode);
			//ar & BOOST_SERIALIZATION_NVP(_nodes);
		}
	};
}
BOOST_CLASS_VERSION(LibCogmatics::Machine, 0)
