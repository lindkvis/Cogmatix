#pragma once

#include <osg/LightSource>
#include "LibCogmatix.h"
#include "Machine.h"

namespace LibCogmatix
{

	class Light : public MachineNode, public osg::LightSource
	{
	protected:
		static unsigned int s_nextLightNumber;
		Machine* _machine;
	public:
		typedef osg::ref_ptr<Light> Ptr;
		typedef osg::ref_ptr<const Light> CPtr;

		void On();
		void Off();
		bool IsOn() const;
	factory_protected:
		Light(NodeID ID, Machine* machine, const Vec& position, const Vec4& colour);
		~Light();
	};
}