#ifndef KRENDERER_H
#define KRENDERER_H

#include "GameObject\KGameObject.h"
#include "TiledMap\KTiledMap.h"
#include "Krawler.h"

namespace Krawler
{
	namespace Renderer
	{
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

			KRAWLER_API void addToRendererQueue(KGameObject* pGameObj);

			KRAWLER_API void render();

			KRAWLER_API bool isInRenderQueue(KGameObject* pGameObj) const;
			KRAWLER_API bool isInRenderQueue(const std::wstring& identifier) const;

			KRAWLER_API void removeFromRenderQueue(KGameObject* pObj);
			KRAWLER_API void removeFromRenderQueue(const std::wstring& identifier);
			KRAWLER_API void setActiveTiledMap(TiledMap::KTiledMap* pTiledMap);

		private:

			void sortByRenderLayer();

			void defaultRender(); 
			void raycastRender(); 

			std::vector<KGameObject*> m_renderQueue;

			KRendererType m_renderingType;

			TiledMap::KTiledMap* mp_tiledMap;

			bool mb_hasTiledMap = false;
		};
	}
}

#endif
