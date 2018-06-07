#ifndef KCRENDERABLE_BASE_H
#define KCRENDERABLE_BASE_H

#include "Krawler.h"
#include "KrawlerLib.h"
#include "KComponent.h"

#include <SFML\Graphics\Shader.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCRenderableBase :
			public KComponentBase, public sf::Drawable
		{
		public:

			KRAWLER_API KCRenderableBase(Krawler::KEntity* pEntity);
			KRAWLER_API virtual ~KCRenderableBase() = default;


			KRAWLER_API void setRenderLayer(Krawler::int32 renderLayer) { m_renderLayer = renderLayer; }
			KRAWLER_API Krawler::int32 getRenderLayer() const { return m_renderLayer; }
			KRAWLER_API sf::Shader* getShader() const { return m_pShader; }
			KRAWLER_API void setShader(sf::Shader* pShader) { m_pShader = pShader; }

			KRAWLER_API virtual Krawler::Rectf getOnscreenBounds() const = 0;

		private:

			Krawler::int32 m_renderLayer = 0;
			sf::Shader* m_pShader = nullptr;
		};
	}
}

#endif

