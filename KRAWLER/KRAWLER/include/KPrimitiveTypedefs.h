#ifndef KPRIMITIVE_TYPEDEFS_H
#define KPRIMITIVE_TYPEDEFS_H

#include <SFML\System\Vector2.hpp>

namespace Krawler
{
	using int8 = signed char;
	using uint8 = unsigned char;

	using int16 = signed short;
	using uint16 = unsigned short;

	using int32 = signed int;
	using uint32 = unsigned int;

	using int64 = signed   __int64;
	using uint64 = unsigned __int64;

	using Vec2f = sf::Vector2f;
	using Vec2i = sf::Vector2i;
	using Vec2u = sf::Vector2u;
	using Vec2d = sf::Vector2u;

	using Point = sf::Vector2f;
}

#endif
