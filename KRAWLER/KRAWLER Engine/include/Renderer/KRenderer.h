#ifndef KRENDERER_H
#define KRENDERER_H

#include "GameObject\KGameObject.h"
#include "Krawler.h"

namespace Krawler
{
	namespace Renderer
	{
		class KRenderer
		{
		public:

			KRenderer() = default;
			~KRenderer();

			void addToRendererQueue(KGameObject* pGameObj);

			void render();

			bool isInRenderQueue(KGameObject* pGameObj) const;
			bool isInRenderQueue(const std::wstring& identifier) const;

			void removeFromRenderQueue(KGameObject* pObj);
			void removeFromRenderQueue(const std::wstring& identifier);

		private:

			void sortByRenderLayer();

			std::vector<KGameObject*> m_renderQueue;

		};
	}
}

#endif
