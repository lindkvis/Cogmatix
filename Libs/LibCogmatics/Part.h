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
		void loadGraphics (const std::string& fileName);
	protected:
		friend class Factory;
		Part(NodeID ID, CoString name, CoString fileName);
		~Part();
	};
}