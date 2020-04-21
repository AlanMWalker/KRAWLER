#ifndef KCCOLLIDER_BASE_H
#define KCCOLLIDER_BASE_H

#include <Krawler.h>	
#include <KComponent.h>
#include <KEntity.h>

#include <Components\KCollisions.h>

#include <functional>
#include <vector>

#include <memory>

class b2Shape;

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

			if (collisionNormal != rhs.collisionNormal && collisionNormal != (rhs.collisionNormal * -1.0f))
			{
				return false;
			}
			return true;
		}
		KEntity* entityA = nullptr;
		KEntity* entityB = nullptr;

		float penetration = 0.0f;
		Vec2f collisionNormal = Vec2f(0.0f, 0.0f);
		uint32 contactCount = 0; 
		Vec2f contacts[2];
	};

	namespace Components
	{
		enum class KCColliderType : int32
		{
			AABB,
			Circle
			// Not currently supported
			//OBB, 
			//Polygon
		};

		struct KCColliderFilteringData
		{
			int16 collisionFilter = 0x0001;
			int16 collisionMask = 0x0001;
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

			KRAWLER_API void setCollisionFilteringData(const KCColliderFilteringData& filteringPOD);

			KRAWLER_API const KCColliderFilteringData& getCollisionFilteringData() const { return m_filterData; }

			std::weak_ptr<b2Shape> getB2Shape() { return std::weak_ptr<b2Shape>(m_pShape); }

		private:

			std::vector<KCColliderBaseCallback*> m_callbacks;

			uint16 m_collisionLayer;

			KCColliderFilteringData m_filterData;
			KCColliderType m_colliderType;

			std::shared_ptr<b2Shape> m_pShape;
		};
	}
}
#endif 
