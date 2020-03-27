#include <functional>

#include "Collisions\KCollisionOverlord.h"
#include "box2d\box2d.h"
#include "KApplication.h"
#include "KScene.h"

#include "Physics\b2dConversion.h"

using namespace Krawler;
using namespace Krawler::Collisions;
using namespace Krawler::Components;

b2AABB rectfToB2ABB(const Rectf& aabb)
{
	b2AABB converted;
	converted.lowerBound.x = aabb.left;
	converted.lowerBound.y = aabb.top;
	converted.upperBound.x = aabb.left + aabb.width;
	converted.upperBound.y = aabb.top + aabb.height;
	return converted;
}

KCollisionOverlord::KCollisionOverlord()
{
	m_pBroadPhase = std::make_unique<b2BroadPhase>();
}

KCollisionOverlord::~KCollisionOverlord()
{
	cleanupProxies();
}

void KCollisionOverlord::triggerSceneCleanup()
{
	cleanupProxies();
}

void KCollisionOverlord::triggerSceneConstruct()
{
	const uint32 ENTITY_COUNT = GET_SCENE()->getNumbrOfEntitiesAllocated();
	auto& entities = GET_SCENE()->getAllocatedEntityList();

	// Iterate over the list of all entites
	for (auto& e : entities)
	{
		// If the entity doesn't have a collider then it won't be added to the 
		// qtree
		auto collider = e->getComponent<KCColliderBase>();
		if (!collider)
		{
			continue;
		}

		ProxyInfo proxy;
		proxy.aabb = new b2AABB(rectfToB2ABB(collider->getBoundingBox()));
		proxy.proxyId = m_pBroadPhase->CreateProxy(*proxy.aabb, nullptr);
		proxy.pEntity = e;
		proxy.pCollider = collider;

		m_proxies.push_back(proxy);
	}
}

void KCollisionOverlord::tick()
{
	relocateProxies();
	checkForProxyInteractions();
}

void KCollisionOverlord::cleanupProxies()
{
	for (auto& proxy : m_proxies)
	{
		proxy.pCollider = nullptr;
		if (proxy.aabb)
		{
			delete proxy.aabb;
			proxy.aabb = nullptr;
		}
	}
	m_proxies.clear();
}

void KCollisionOverlord::relocateProxies()
{
	static bool bHasTickedOnce = false;
	for (auto& proxy : m_proxies)
	{
		if (!bHasTickedOnce)
		{
			proxy.lastPos = proxy.pEntity->getTransform()->getTranslation();
		}
		else
		{
			const Vec2f currentPosition = proxy.pEntity->getTransform()->getTranslation();
			const Vec2f displacement = currentPosition - proxy.lastPos;
			// if no displacement we can continue the main loop
			if (displacement == Vec2f(0.0f, 0.0f))
			{
				continue;
			}

			(*proxy.aabb) = b2AABB(rectfToB2ABB(proxy.pCollider->getBoundingBox()));
			// Note MoveProxy returns a bool for if the object was reinserted
			// this may be useful within the confines of the world
			m_pBroadPhase->MoveProxy(proxy.proxyId, *proxy.aabb, Vec2fTob2(displacement));
		}
	}

	if (!bHasTickedOnce)
	{
		bHasTickedOnce = true;
	}
}

void KCollisionOverlord::checkForProxyInteractions()
{
	for (auto& p : m_proxies)
	{
		if (!p.pEntity->isEntityInUse())
		{
			continue;
		}
		
		auto queryCallback = [](int32 proxyId) -> void {

		};

		m_pBroadPhase->Query(&p,*p.aabb);
	}
}
