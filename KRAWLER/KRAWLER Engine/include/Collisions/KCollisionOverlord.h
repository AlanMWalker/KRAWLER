#ifndef KCOLLISION_OVERLORD_H
#define KCOLLISION_OVERLORD_H

#include <vector>
#include <memory>

#include "Krawler.h"
#include "Components\KCColliderBase.h"

#include <unordered_set>

class b2DynamicTree;
class b2AABB;
class b2BroadPhase;
class b2RayCastInput;

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
			
			KRAWLER_API void castRayInScene(const Vec2f& start, const Vec2f& end, KEntity* pCastingEntity = nullptr);

		private:
			using ProxyPair = std::pair<int32, int32>;
			struct ProxyInfo
			{
				int32 proxyId;
				Components::KCColliderBase* pCollider;
				b2AABB* aabb;
				Vec2f lastPos;
				KEntity* pEntity;
				std::vector<ProxyPair>* pToCheck;

				bool QueryCallback(int neighbourProxyId)
				{
					if (neighbourProxyId == proxyId)
					{
						// we've stumbled upon ourselves
						return true;
					}
					//possibleIntersections.push(neighbourProxyId);
					pToCheck->push_back(ProxyPair(proxyId, neighbourProxyId));
					return true;
				}
			};

			struct RaycastCB
			{
				int castingID = -1;
				float RayCastCallback(const b2RayCastInput& input, int id);
			};

			void cleanupProxies();

			// Handles the movement of proxies withing the dynamic trees
			void relocateProxies();

			// will force query calls for each entity
			void checkForProxyInteractions();

			// will perform in-depth narrow phase by using collider 
			void performNarrowPhaseForProxies();

			void generateNarrowPhaseQueue();

			int32 getProxyIndexFromId(int32 proxyId) const;

			std::unique_ptr<b2BroadPhase> m_pBroadPhase;
			
			std::vector<ProxyInfo> m_proxies;
			std::vector<ProxyPair> m_intersectionsToCheck;
			std::deque<ProxyPair> m_narrowPhaseQueue;
		};
	}
}

#endif