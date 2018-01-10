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

			KDEPRECATED(KRAWLER_API void addToRendererQueue)(KGameObject* pGameObj);

			KRAWLER_API void render();

			KDEPRECATED(KRAWLER_API bool isInRenderQueue)(KGameObject* pGameObj) const;
			KDEPRECATED(KRAWLER_API bool isInRenderQueue)(const std::wstring& identifier) const;

			KDEPRECATED(KRAWLER_API void removeFromRenderQueue)(KGameObject* pObj);
			KDEPRECATED(KRAWLER_API void removeFromRenderQueue)(const std::wstring& identifier);

			KDEPRECATED(KRAWLER_API void clearRenderQueue)();

			KRAWLER_API void setActiveTiledMap(TiledMap::KTiledMap* pTiledMap);

			KRAWLER_API KDEPRECATED(int32 addTextToScreen)(const sf::Text& pText, const Vec2i& screenPos)
			{
				m_screenText.push_back(TextRender(screenPos, pText));
				m_screenText.back().second.setFont(mp_defaultFont);
				return (int32)m_screenText.size() - 1;
			}

			KRAWLER_API KDEPRECATED(sf::Text& getTextByIndex)(int32 i) { return m_screenText[i].second; }

		private:
			void generateSpriteList();
			void sortByRenderLayer();

			void defaultRender();
			void raycastRender();

			Vec2f screenToWorld(const Vec2i& vec) const;

			std::vector<KGameObject*> m_renderQueue;
			std::vector<Components::KCSprite*> m_sprites;
			std::vector<TextRender>m_screenText;

			KRendererType m_renderingType;

			TiledMap::KTiledMap* mp_tiledMap;

			bool mb_hasTiledMap = false;
			sf::Font mp_defaultFont;

		};
	}
}

#endif
