#ifndef KRENDERER_H
#define KRENDERER_H

#include "Krawler.h"
#include "Components\KCSprite.h"

#include "GameObject\KGameObject.h"
#include "TiledMap\KTiledMap.h"

namespace Krawler
{
	namespace Renderer
	{
		using TextRender = std::pair < Vec2i, sf::Text>;
		using TileMapRenderItem = std::pair<int32, TiledMap::KTiledMap*>;
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

			KRAWLER_API void addTiledMap(int32 renderOrder, TiledMap::KTiledMap* pTiledMap);

			KRAWLER_API const std::vector<TileMapRenderItem>& getTileMapRenderList() const { return m_tiledMaps; }


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

			std::vector<KGameObject*> m_renderQueue;
			std::vector<Components::KCRenderableBase*> m_renderable;
			std::vector<TextRender>m_screenText;
			std::vector <TileMapRenderItem> m_tiledMaps;
			KRendererType m_renderingType;

			TiledMap::KTiledMap* mp_tiledMap;

			bool mb_hasTiledMap = false;
			sf::Font mp_defaultFont;

		};
	}
}

#endif
