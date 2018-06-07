#ifndef KRENDERER_H
#define KRENDERER_H

#include "Krawler.h"
#include "Components\KCSprite.h"
#include <SFML\Graphics\Text.hpp>	
#include <SFML\Graphics\Font.hpp>

namespace Krawler
{
	namespace Renderer
	{
		using TextRender = std::pair < Vec2i, sf::Text>;
		enum KRendererType : uint32
		{
			Default,
			Raycast
		};

		class KRenderer
		{
		public:

			KRAWLER_API KRenderer();
			KRAWLER_API ~KRenderer();

			KRAWLER_API void render();

			KRAWLER_API KDEPRECATED(int32 addTextToScreen)(const sf::Text& pText, const Vec2i& screenPos)
			{
				m_screenText.push_back(TextRender(screenPos, pText));
				m_screenText.back().second.setFont(mp_defaultFont);
				return (int32)m_screenText.size() - 1;
			}

			KRAWLER_API KDEPRECATED(sf::Text& getTextByIndex)(int32 i) { return m_screenText[i].second; }

		private:
			void generateRenderableList();
			void sortByRenderLayer();

			void defaultRender();

			Vec2f screenToWorld(const Vec2i& vec) const;

			std::vector<Components::KCRenderableBase*> m_renderable;
			std::vector<TextRender>m_screenText;
			KRendererType m_renderingType;


			bool mb_hasTiledMap = false;
			sf::Font mp_defaultFont;

		};
	}
}

#endif
