#ifndef KCTRANSFORM_H
#define KCTRANSFORM_H

#include "Krawler.h"
#include "KComponent.h"
#include "KEntity.h"
#include <SFML\Graphics\Transform.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCTransform : public KComponentBase
		{
		public:

			KRAWLER_API KCTransform(KEntity* pEntity);
			KRAWLER_API ~KCTransform() = default;

			KRAWLER_API virtual void tick() override;

			KRAWLER_API const sf::Transform& getTransform();
			KRAWLER_API void setParent(KEntity* pEntity);

			KRAWLER_API void setRotation(float angleInDeg);

			KRAWLER_API void setOrigin(const Vec2f& origin);
			KRAWLER_API void setOrigin(float x, float y);

			KRAWLER_API void setTranslation(const Vec2f& trans);
			KRAWLER_API void setTranslation(float dx, float dy);

			KRAWLER_API const Vec2f getPosition() const;
			KRAWLER_API float getRotation() const;

			KRAWLER_API void setScale(const Vec2f& scale);
			KRAWLER_API void setScale(float x, float y);
			KRAWLER_API Vec2f getScale() const;

			KRAWLER_API void move(float dx, float dy);
			KRAWLER_API void move(const Vec2f& trans);

			KRAWLER_API void rotate(float angleInDeg);

			KRAWLER_API void scale(float sx, float sy);
			KRAWLER_API void scale(const Vec2f& scale);

			//get local scale
			//get local rot
			// get local trans

		private:

			void reconstructTransform();

			KCTransform* m_pParentTransform;

			sf::Transform m_transform;
			sf::Transform m_parentedTransform;

			float m_rotation;

			Vec2f m_origin;
			Vec2f m_scale;
			Vec2f m_trans;

			Vec2f m_worldPos;
			Vec2f m_worldScale;
			float m_worldRot;

			bool m_bHasParent;
			bool m_bUpdateTransform;
		};
	}
}
#endif 
