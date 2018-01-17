#ifndef KCSPRITE_H
#define KCSPRITE_H

#include "Krawler.h"
#include "KCTransform.h"

#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\Shader.hpp>

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
			KRAWLER_API void setColour(const Colour& col);

			KRAWLER_API void setTexture(sf::Texture* pTexture);
			KRAWLER_API void setTextureRect(const Recti& texRect);

			KRAWLER_API Rectf getOnscreenBounds() const;

			KRAWLER_API sf::Shader* getShader() { return m_pShader; }
			
			KRAWLER_API void setShader(sf::Shader* pShader) { m_pShader = pShader; }
		private:

			sf::VertexArray m_vertexArray;
			sf::Vector2f m_size;

			sf::Texture* m_pTexture;
			sf::Shader* m_pShader;
			KCTransform* m_pTransform;

			int32 m_renderLayer;

		};
	}
}

#endif
