#ifndef KCBOX_COLLIDER_H
#define KCBOX_COLLIDER_H

#include "Krawler.h"
#include "KCColliderBase.h"

class b2PolygonShape; 

namespace Krawler
{
	namespace Components
	{
		class KCTransform;
		class KCBoxCollider : public KCColliderBase
		{
		public:

			KRAWLER_API KCBoxCollider(KEntity* pEntity, const Vec2f& size);
			KRAWLER_API ~KCBoxCollider() = default;

			KRAWLER_API KDEPRECATED_FUNC(const Rectf& getBounds)();
			KRAWLER_API virtual const Rectf& getBoundingBox() override;
			KRAWLER_API const Vec2f& getHalfSize() const { return m_halfSize; }
			KRAWLER_API	Vec2f getTopLeftCoord() const;

		private:

			void updateAABB();

			Vec2f m_size;
			Vec2f m_halfSize;
			Rectf m_aabb;

			std::weak_ptr<b2PolygonShape> m_pBoxShapeb2;
			
			KCTransform* m_pTransform;
		};
	}
}
#endif
