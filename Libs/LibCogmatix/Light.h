#pragma once

#include <osg/LightSource>
#include "LibCogmatix.h"
#include "Machine.h"

namespace LibCogmatix
{

	class Light : public TMachineNode<osg::LightSource>
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
        Light(const Light& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
            : TMachineNode<osg::LightSource>(copyFrom, copyop)
        {
        }
        Light() : TMachineNode<osg::LightSource>() {}
		~Light();
        META_Node (LibCogmatix, Light);
	};
}