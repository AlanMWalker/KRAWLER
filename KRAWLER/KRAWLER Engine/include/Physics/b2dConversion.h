#ifndef B2DCONVERSION_H
#define B2DCONVERSION_H

#include "box2d/box2d.h"
#include "Krawler.h"
// Utility Conversion Functions for Physics
Krawler::Vec2f b2ToVec2f(const b2Vec2& in);
b2Vec2 Vec2fTob2(const Krawler::Vec2f& in);
#endif