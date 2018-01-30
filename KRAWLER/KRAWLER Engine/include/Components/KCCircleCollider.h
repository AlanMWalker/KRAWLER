#ifndef KCCIRCLE_COLLIDER_H
#define KCCIRCLE_COLLIDER_H

#include "Krawler.h"
#include "KCColliderBase.h"
namespace Krawler
{
	namespace Components
	{
		class KCCircleCollider : public KCColliderBase
		{
		public:

			KRAWLER_API KCCircleCollider(KEntity* pEntity, float radius);
			KRAWLER_API ~KCCircleCollider() = default;

			KRAWLER_API const Vec2f& getCentrePosition();
			KRAWLER_API float getRadius() const;
			KRAWLER_API virtual const Rectf& getBoundingBox() override;

		private:

			void updateCentrePosition();

			float m_radius;
			Vec2f m_centrePos;
			Rectf m_boundingBox;

		};
	}
}
#endif 