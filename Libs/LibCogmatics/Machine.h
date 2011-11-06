#pragma once

#include <osg/MatrixTransform>
#include <boost/serialization/vector.hpp>
#include <Boost/serialization/shared_ptr.hpp>

#include "Node.h"

namespace LibCogmatics
{
	class Machine : public NamedMachineNode, public osg::MatrixTransform	
	{
	protected:
		friend class Factory;
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
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(NamedMachineNode);
		}
	};
}
BOOST_CLASS_VERSION(LibCogmatics::Machine, 0)
