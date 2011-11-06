#include "StdAfx.h"

#include <osgDB/ReadFile> 

#include "Part.h"

namespace LibCogmatics
{
	Part::Part(NodeID ID, CoString name, CoString fileName) : NamedMachineNode(ID, name) 
	{
		loadGraphics(fileName);
	}

	Part::~Part()
	{
	}

	void Part::loadGraphics (CoString fileName)
	{
		_fileName = fileName;
		osg::Node* node = osgDB::readNodeFile(_fileName);
		addChild(node);
	}
}