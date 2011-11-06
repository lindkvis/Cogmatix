#pragma once

#include <osg/Geode>

#include "node.h"

namespace LibCogmatics
{
	class Part : public NamedMachineNode, public osg::MatrixTransform
	{
	public:
		typedef	osg::ref_ptr<Part> Ptr;
		typedef osg::ref_ptr<const Part> CPtr;
		// TODO: make this unicode
		void loadGraphics (CoString fileName);
	protected:
		friend class Factory;
		String _fileName;

		Part(NodeID ID, CoString name, CoString fileName);
		~Part();
	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(NamedMachineNode);
			ar & BOOST_SERIALIZATION_NVP(_fileName);
		}
	};
}