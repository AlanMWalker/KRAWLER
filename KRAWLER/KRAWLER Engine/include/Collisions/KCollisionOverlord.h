#ifndef KCOLLISION_OVERLORD_H
#define KCOLLISION_OVERLORD_H

#include <vector>

#include "Krawler.h"
#include "Components\KCColliderBase.h"

class b2DynamicTree;
class b2AABB;

namespace Krawler
{
	namespace Collisions
	{
		/* Engine internal use only class. 
		Won't be exposed through DLL. */
		class KCollisionOverlord
		{
		public:

			KCollisionOverlord();
			~KCollisionOverlord();

			// Triggered when onExitScene() is called to clean up the tree 
			void triggerSceneCleanup();

			// Triggered after onEnterScene() to construct the tree
			void triggerSceneConstruct();

			// Update proxy positions on tick
			void tick();


		private:
			struct ProxyInfo
			{
				int32 proxyId;
				Krawler::Components::KCColliderBase* pCollider;
				b2AABB* aabb;
			};

			void cleanupProxies();

			b2DynamicTree* m_pDynamicTree;
			std::vector<ProxyInfo> m_proxies;
		};
	}
}

#endif