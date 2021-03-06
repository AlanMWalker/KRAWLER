#ifndef KCSPRITE_H
#define KCSPRITE_H

#include "Krawler.h"
#include "KCTransform.h"
#include "KCRenderableBase.h"

#include <SFML\Graphics\VertexArray.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCSprite : public KCRenderableBase
		{
		public:

			KRAWLER_API KCSprite(KEntity* pEntity, const Vec2f& size = Vec2f(1.0f, 1.0f));
			KRAWLER_API ~KCSprite() = default;

			KRAWLER_API virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const override;

			KRAWLER_API void setColour(const Colour& col);

			//Sets texture for this sprite 
			//(side effect of changing texture rect on use)
			KRAWLER_API void setTexture(sf::Texture* pTexture);
			KRAWLER_API void setTextureRect(const Recti& texRect);

			KRAWLER_API virtual Rectf getOnscreenBounds() const override;

			KRAWLER_API void operator = (const KCSprite& spr);

		private:

			sf::VertexArray m_vertexArray;
			sf::Vector2f m_size;
			sf::Texture* m_pTexture;
			KCTransform* m_pTransform;

		};
	}
}

#endif
