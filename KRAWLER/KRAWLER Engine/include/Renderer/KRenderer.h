#ifndef KRENDERER_H
#define KRENDERER_H

#include "Krawler.h"
#include "Components\KCSprite.h"
#include "Components\KCTileMap.h"
#include <SFML\Graphics\Text.hpp>	
#include <SFML\Graphics\Font.hpp>

#include <functional>

namespace Krawler
{
	namespace Renderer
	{
		using TextRender = std::pair <Vec2i, sf::Text>;
		enum KRendererType : uint32
		{
			Default,
			Raycast
		};

		enum class KRenderSortType
		{
			LayerSort,
			ZOrderSort
		};

		class KRenderer
		{
		public:

			KRAWLER_API KRenderer();
			KRAWLER_API ~KRenderer();

			KRAWLER_API void render();

			KRAWLER_API void setSortType(KRenderSortType sortType) { m_sortType = sortType; }

			KRAWLER_API KRenderSortType getSortType() const { return m_sortType; }

			KRAWLER_API KDEPRECATED(int32 addTextToScreen)(const sf::Text& pText, const Vec2i& screenPos)
			{
				m_screenText.push_back(TextRender(screenPos, pText));
				m_screenText.back().second.setFont(mp_defaultFont);
				return (int32)m_screenText.size() - 1;
			}

			KRAWLER_API KDEPRECATED(sf::Text& getTextByIndex)(int32 i) { return m_screenText[i].second; }

			KRAWLER_API void subscribeLastDrawCallback(std::function<void(void)> func); // subscribe a callback functions to be triggered as the last draw calls post entity drawing

		private:
			void generateRenderableList();
			void sortByRenderLayer();
			void sortByZOrder();
			void defaultRender();

			Vec2f screenToWorld(const Vec2i& vec) const;

			std::vector<Components::KCRenderableBase*> m_renderablesVector;
			std::vector<Components::KCTileMapSplit*> m_splitMapVec;
			std::vector<TextRender>m_screenText;
			std::vector<std::function<void(void)>> m_lastDrawCallbacks;

			KRendererType m_renderingType;
			KRenderSortType m_sortType;

			bool mb_hasTiledMap = false;
			sf::Font mp_defaultFont;

		};
	}
}

#endif
