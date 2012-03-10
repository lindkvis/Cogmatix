#include "StdAfx.h"

#include <osgDB/ReadFile> 

#include "CompositePart.h"

namespace LibCogmatix
{
	CompositePart::CompositePart(NodeID ID, CoString name, CoString fileName) : TMachineNode(ID)
	{
		loadGraphics(fileName);
        setName(name);
	}

	CompositePart::~CompositePart()
	{
	}

	void CompositePart::loadGraphics (CoString fileName)
	{
		_fileName = fileName;
		osg::Node* node = osgDB::readNodeFile(_fileName);
		addChild(node);
	}
}