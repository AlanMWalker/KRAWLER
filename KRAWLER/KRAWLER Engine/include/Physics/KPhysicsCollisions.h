#ifndef KPHYSICS_COLLISIONS_H
#define KPHYSICS_COLLISIONS_H

#include "KPhysicsStructs.h"
#include "KrawlerLib.h"

namespace Krawler
{
	namespace Physics
	{
		bool CirclevsCircleCollision(KCollisionData& collData);

		bool AABBvsAABBCollision(KCollisionData& collData);

		bool AABBvsCircleCollision(KCollisionData & collData);

		bool CircleVsAABBCollision(KCollisionData & collData);

		bool PolygonvsPolygon(KCollisionData& collData);

	}
}
#endif 
