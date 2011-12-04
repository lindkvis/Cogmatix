#pragma once

#include <exception>
#include "LibCogmatix.h"

namespace LibCogmatix
{
	class CogException : public std::exception
	{
	public:
		enum Type
		{
			Custom=0,		
			BadParameter
		};

		CogException (Type type, CoString text) : _type(type), _text(text) {}
		~CogException(void) {}

	private:
		std::string _text; /// <Custom text>
		Type _type; /// <Exception number>
	};
}

#define EXCEPT_IF(predicate, type, text) \
	if (predicate) \
		throw CogException(type, text);