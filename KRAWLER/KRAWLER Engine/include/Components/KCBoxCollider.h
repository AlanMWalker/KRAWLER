#ifndef KCBOX_COLLIDER_H
#define KCBOX_COLLIDER_H

#include "Krawler.h"
#include "KComponent.h"

#include <vector>
#include <functional>
#include <SFML\Graphics\Rect.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCBoxCollider : public KComponentBase
		{
		public:

			KRAWLER_API KCBoxCollider(KEntity* pEntity, const Vec2f& size);
			KRAWLER_API ~KCBoxCollider() = default;

			KRAWLER_API void collisionCallback(KEntity* pEntity);
			KRAWLER_API virtual void tick() override;

			KRAWLER_API bool checkIntersects(KCBoxCollider* pCollider);

			KRAWLER_API const sf::FloatRect& getBounds() const { return m_aabb; }

			KRAWLER_API void subscribeCollisionCallback(std::function<void(KEntity*)> callback)
			{
				m_callbacks.push_back(callback);
			};

		private:

			void updateAABB();

			Vec2f m_globalVertices[4];
			Vec2f m_localVertices[4];
			//Vector of lambda callbacks
			std::vector<std::function<void(KEntity*)>> m_callbacks;

			Vec2f m_size;
			sf::FloatRect m_aabb;
		};
	}
}
#endif
