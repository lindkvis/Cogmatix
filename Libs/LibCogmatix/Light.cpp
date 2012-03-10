#include "StdAfx.h"
#include "Light.h"

namespace LibCogmatix
{
	unsigned int Light::s_nextLightNumber = 0;
	Light::Light(NodeID ID, Machine* machine, const Vec& position, const Vec4& colour) : TMachineNode(ID), _machine(machine)
	{
		osg::ref_ptr<osg::Light> light = new osg::Light();
		light->setLightNum(s_nextLightNumber++);
		light->setDiffuse(colour);
		light->setSpecular(Vec4(1.f, 1.f, 1.f, 1.f));
		light->setAmbient(Vec4(0.1f, 0.1f, 0.1f, 1.0f));
		light->setPosition(Vec4(position[0], position[1], position[2], 1.));
		setLight(light);
		On();
	}

	Light::~Light(void)
	{
	}

	void Light::On ()
	{
		setLocalStateSetModes(osg::StateAttribute::ON);
		setStateSetModes(*_machine->getOrCreateStateSet(), osg::StateAttribute::ON);
	}

	void Light::Off ()
	{
		setLocalStateSetModes(osg::StateAttribute::OFF);
		setStateSetModes(*_machine->getOrCreateStateSet(), osg::StateAttribute::OFF);
	}

	bool Light::IsOn() const
	{
		return (_value == osg::StateAttribute::ON);
	}
}