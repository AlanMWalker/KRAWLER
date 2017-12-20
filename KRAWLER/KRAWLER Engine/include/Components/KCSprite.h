#ifndef KCSPRITE_H
#define KCSPRITE_H

#include "Krawler.h"
#include "KCTransform.h"

#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\VertexArray.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCSprite : public KComponentBase, public sf::Drawable
		{
		public:

			KRAWLER_API KCSprite(KEntity* pEntity, const Vec2f& size = Vec2f(1.0f, 1.0f));
			KRAWLER_API ~KCSprite() = default;

			KRAWLER_API virtual KInitStatus init() override;
			virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const override;

			KRAWLER_API void setRenderLayer(int32 renderLayer) { m_renderLayer = renderLayer; }

			KRAWLER_API int32 getRenderLayer() const { return m_renderLayer; }

			//TODO set col
			//TODO set texture rect
			//TODO set texture

		private:

			sf::VertexArray m_vertexArray;
			sf::Vector2f m_size;
			
			sf::Texture* m_pTexture;
			KCTransform* m_pTransform;

			int32 m_renderLayer;

		};
	}
}

#endif
