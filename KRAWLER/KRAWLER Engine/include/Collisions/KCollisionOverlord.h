#ifndef KCOLLISION_OVERLORD_H
#define KCOLLISION_OVERLORD_H

#include <vector>
#include <memory>

#include "Krawler.h"
#include "Components\KCColliderBase.h"

class b2DynamicTree;
class b2AABB;
class b2BroadPhase;

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
				Components::KCColliderBase* pCollider;
				b2AABB* aabb;
				Vec2f lastPos;
				KEntity* pEntity;
				bool QueryCallback(int proxyId)
				{
					KPrintf(L"Possible intersection with %d\n");
					return true;
				}
			};

			void cleanupProxies();

			// Handles the movement of proxies withing the dynamic trees
			void relocateProxies();
			
			void checkForProxyInteractions();

			std::unique_ptr<b2BroadPhase> m_pBroadPhase;
			std::vector<ProxyInfo> m_proxies;
		};
	}
}

#endif