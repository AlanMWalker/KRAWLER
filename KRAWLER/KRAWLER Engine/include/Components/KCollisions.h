#ifndef KCOLLISIONS_H
#define KCOLLISIONS_H

#include <Krawler.h>

namespace Krawler
{
	struct KCollisionDetectionData;

	KRAWLER_API bool AABBvsAABB(KCollisionDetectionData& data);
	KRAWLER_API bool CirclevsCircle(KCollisionDetectionData& data);
	KRAWLER_API bool AABBvsCircle(KCollisionDetectionData& data);
	KRAWLER_API bool CirclevsAABB(KCollisionDetectionData& data);

	KRAWLER_API bool OBBvsAABB(KCollisionDetectionData& data);
	KRAWLER_API bool AABBvsOBB(KCollisionDetectionData& data);
	KRAWLER_API bool OBBvsCirlce(KCollisionDetectionData& data);
	KRAWLER_API bool CirclevsOBB(KCollisionDetectionData& data);
	KRAWLER_API bool OBBvsOBB(KCollisionDetectionData& data);

	constexpr int32 MAX_COLLIDER_TYPE = 3;
	static bool(*CollisionLookupTable[MAX_COLLIDER_TYPE][MAX_COLLIDER_TYPE])(KCollisionDetectionData&) =
	{
		{ AABBvsAABB, AABBvsCircle, AABBvsOBB },
		{ CirclevsAABB, CirclevsCircle, CirclevsOBB },
		{ OBBvsAABB, OBBvsCirlce, OBBvsOBB }
	};


	

}
#endif 