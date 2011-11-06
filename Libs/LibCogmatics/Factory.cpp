#include "StdAfx.h"
#include "Factory.h"

namespace LibCogmatics
{
	// Initialise the static pointer.
	Factory* Factory::m_pInstance = nullptr;

	Factory* Factory::Get()
	{
		if (nullptr==m_pInstance)
			m_pInstance = new Factory();
		return m_pInstance;
	}

	LinearAxis::Ptr Factory::CreateLinearAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max)
	{
		return LinearAxis::Ptr (new LinearAxis (_nextID++, axisVector, origin, valueInitial, min, max));
	}

	RotaryAxis::Ptr Factory::CreateRotaryAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max)
	{
		return RotaryAxis::Ptr (new RotaryAxis (_nextID++, axisVector, origin, valueInitial, min, max));
	}

	Machine::Ptr Factory::CreateMachine(CoString name)
	{
		return Machine::Ptr (new Machine (_nextID++, name));
	}

	Part::Ptr Factory::CreatePart(CoString name, CoString fileName)
	{
		return Part::Ptr (new Part (_nextID++, name, fileName));
	}
}