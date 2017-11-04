#ifndef KPHYSICS_COLLISIONS_H
#define KPHYSICS_COLLISIONS_H

#include "KPhysicsStructs.h"
#include "KrawlerLib.h"

namespace Krawler
{
	namespace Physics
	{
		bool CirclevsCircleCollision(CollisionData& collData);

		bool AABBvsAABBCollision(CollisionData& collData);

		bool AABBvsCircleCollision(CollisionData & collData);

		bool CircleVsAABBCollision(CollisionData & collData);
	}
}
#endif 
