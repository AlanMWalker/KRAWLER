#include "Collisions\KCollisionOverlord.h"
#include "box2d\box2d.h"
#include "KApplication.h"
#include "KScene.h"

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
	m_pDynamicTree = new b2DynamicTree();
	assert(m_pDynamicTree);
}

KCollisionOverlord::~KCollisionOverlord()
{
	if (m_pDynamicTree)
	{
		delete m_pDynamicTree;
		m_pDynamicTree = nullptr;
	}

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
		proxy.proxyId = m_pDynamicTree->CreateProxy(*proxy.aabb, nullptr);
		m_proxies.push_back(proxy);
	}
}

void KCollisionOverlord::tick()
{
	for (auto& proxy : m_proxies)
	{
		//m_pDynamicTree->MoveProxy()
	}
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

