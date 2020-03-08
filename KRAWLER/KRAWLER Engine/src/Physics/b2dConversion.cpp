#include "Physics/b2dConversion.h"

using namespace Krawler;

// Utility Conversion Functions
Vec2f b2ToVec2f(const b2Vec2& in)
{
	return Vec2f(in.x, in.y);
}

b2Vec2 Vec2fTob2(const Vec2f& in)
{
	return b2Vec2(in.x, in.y);
}