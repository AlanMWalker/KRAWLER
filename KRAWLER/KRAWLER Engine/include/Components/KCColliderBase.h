#ifndef KCCOLLIDER_BASE_H
#define KCCOLLIDER_BASE_H

#include "Krawler.h"	
#include "KComponent.h"	
#include "KEntity.h"

#include <functional>
#include <vector>

namespace Krawler
{

	struct KCollisionDetectionData
	{
		KRAWLER_API bool operator == (const KCollisionDetectionData& rhs) const
		{
			if (entityA != rhs.entityA && entityA != rhs.entityB)
			{
				return false;
			}

			if (entityB != rhs.entityB && entityB != rhs.entityA)
			{
				return false;
			}

			if (penetration != rhs.penetration)
			{
				return false;
			}

			if (collisionNormal != rhs.collisionNormal && collisionNormal != (rhs.collisionNormal*-1.0f))
			{
				return false;
			}
			return true;
		}
		KEntity* entityA = nullptr;
		KEntity* entityB = nullptr;

		float penetration = 0.0f;
		Vec2f collisionNormal = Vec2f(0.0f, 0.0f);
	};


	KRAWLER_API bool AABBvsAABB(KCollisionDetectionData& data);
	KRAWLER_API bool CirclevsCircle(KCollisionDetectionData& data);
	KRAWLER_API bool AABBvsCircle(KCollisionDetectionData& data);
	KRAWLER_API bool CirclevsAABB(KCollisionDetectionData& data);

	static bool(*CollisionLookupTable[2][2])(KCollisionDetectionData&) =
	{
		{ AABBvsAABB, AABBvsCircle },
		{ CirclevsAABB, CirclevsCircle },
	};


	namespace Components
	{
		enum KCColliderType : int32
		{
			AABB,
			Circle,
			Mesh
		};

		using KCColliderBaseCallback = std::function<void(const KCollisionDetectionData& collData)>;

		class KCColliderBase : public KComponentBase
		{
		public:

			KRAWLER_API KCColliderBase(KEntity* pEntity, KCColliderType type);
			KRAWLER_API virtual ~KCColliderBase() = default;

			KRAWLER_API KCColliderType getColliderType() const { return m_colliderType; }

			KRAWLER_API int32 subscribeCollisionCallback(KCColliderBaseCallback* callback);

			KRAWLER_API int32 unsubscribeCollisionCallback(KCColliderBaseCallback* callback);

			KRAWLER_API void collisionCallback(const KCollisionDetectionData& collData);

			KRAWLER_API bool isCallbackSubscribed(KCColliderBaseCallback* callback) const;

			KRAWLER_API virtual const Rectf& getBoundingBox() = 0;

			KRAWLER_API void setCollisionLayer(int16 collisionLayer);

			KRAWLER_API int16 getCollisionLayer() const { return m_collisionLayer; }

		private:

			std::vector<KCColliderBaseCallback*> m_callbacks;

			uint16 m_collisionLayer;

			KCColliderType m_colliderType;
		};
	}
}
#endif 
