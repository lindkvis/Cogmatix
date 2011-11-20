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

	CompositePart::Ptr Factory::CreateCompositePart(CoString name, CoString fileName)
	{
		return CompositePart::Ptr (new CompositePart (_nextID++, name, fileName));
	}

	Part::Ptr Factory::CreatePart(CoString name)
	{
		return Part::Ptr (new Part (_nextID++, name));
	}

	ParametricSpurGear::Ptr Factory::CreateParametricSpurGear(short numberOfTeeth, Length depth, Length axisDiameter, Length module)
	{
		return ParametricSpurGear::Ptr (new ParametricSpurGear (numberOfTeeth, depth, axisDiameter, module));
	}


}