#include "StdAfx.h"

#include <osgDB/ReadFile> 

#include "CompositePart.h"

namespace LibCogmatix
{
	void CompositePart::loadGraphics (CoString fileName)
	{
		_fileName = fileName;
		osg::Node* node = osgDB::readNodeFile(_fileName);
		addChild(node);
	}
}