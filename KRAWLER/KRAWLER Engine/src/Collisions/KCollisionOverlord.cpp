#include <functional>
#include <stack>

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
		proxy.pToCheck = &m_intersectionsToCheck;

		m_proxies.push_back(proxy);
	}
}

void KCollisionOverlord::tick()
{
	relocateProxies();
	checkForProxyInteractions();
	performNarrowPhaseForProxies();
}

void Krawler::Collisions::KCollisionOverlord::castRayInScene(const Vec2f& start, const Vec2f& end, KEntity* pCastingEntity)
{
	b2RayCastInput input;
	input.maxFraction = 1.0f;
	input.p1 = Vec2fTob2(start);
	input.p2 = Vec2fTob2(end);

	RaycastCB cb;
	for (auto& p : m_proxies)
	{
		if (p.pEntity == pCastingEntity)
		{
			cb.castingID = p.proxyId;
		}
	}
	m_pBroadPhase->RayCast(&cb, input);
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
	m_intersectionsToCheck.clear();
	for (auto& p : m_proxies)
	{
		if (!p.pEntity->isEntityAwake())
		{
			continue;
		}
		m_pBroadPhase->TouchProxy(p.proxyId);
	}
	for (auto& p : m_proxies)
	{
		m_pBroadPhase->Query(&p, *p.aabb);
	}
}

void KCollisionOverlord::performNarrowPhaseForProxies()
{
	// We need the narrow phase queue to be populated before we do in depth searches
	generateNarrowPhaseQueue();
	for (auto pair : m_narrowPhaseQueue)
	{
		const ProxyInfo& proxyA = m_proxies[getProxyIndexFromId(pair.first)];
		const ProxyInfo& proxyB = m_proxies[getProxyIndexFromId(pair.second)];

		// Need as raw pointer since box2d uses raw
		b2Shape* pShapeA = proxyA.pCollider->getB2Shape().lock().get();
		b2Shape* pShapeB = proxyB.pCollider->getB2Shape().lock().get();

		b2Transform transA = proxyA.pCollider->getB2Transform();
		b2Transform transB = proxyB.pCollider->getB2Transform();

		const b2Vec2 positionA = Vec2fTob2(proxyA.pEntity->getTransform()->getTranslation());
		const float rotationA = Maths::Radians(proxyA.pEntity->getTransform()->getRotation());
		transA.Set(positionA, rotationA);

		const b2Vec2 positionB = Vec2fTob2(proxyB.pEntity->getTransform()->getTranslation());
		const float rotationB = Maths::Radians(proxyB.pEntity->getTransform()->getRotation());
		transB.Set(positionB, rotationB);

		const int32 colliderTypeA = static_cast<int32>(proxyA.pCollider->getColliderType());
		const int32 colliderTypeB = static_cast<int32>(proxyB.pCollider->getColliderType());

		KCollisionDetectionData d;
		d.entityA = proxyA.pEntity;
		d.entityB = proxyB.pEntity;

		/*	const auto collision = CollisionLookupTable[colliderTypeA][colliderTypeB](d);
			if (!collision)
			{
				continue;
			}*/

		auto collision = b2TestOverlap(pShapeA, 0, pShapeB, 0, transA, transB);
		if (!collision)
		{
			continue;
		}

		KCollisionDetectionData data;
		data.entityA = proxyA.pEntity;
		data.entityB = proxyB.pEntity;

		b2Manifold manifoldA;
		b2Manifold manifoldB;


		proxyA.pCollider->collisionCallback(data);
		proxyB.pCollider->collisionCallback(data);
	}
}

void KCollisionOverlord::generateNarrowPhaseQueue()
{
	m_narrowPhaseQueue.clear();
	for (auto& pair : m_intersectionsToCheck)
	{
		const auto result = std::find_if(m_narrowPhaseQueue.begin(), m_narrowPhaseQueue.end(), [pair](const ProxyPair& toCheckPair) -> bool
			{
				if (pair == toCheckPair)
				{
					return true;
				}

				if (pair == ProxyPair(toCheckPair.second, toCheckPair.first))
				{
					return true;
				}

				return false;
			});

		if (result == m_narrowPhaseQueue.end())
		{
			m_narrowPhaseQueue.push_back(pair);
		}
	}
}

int32 KCollisionOverlord::getProxyIndexFromId(int32 proxyId) const
{
	const auto result = std::find_if(m_proxies.begin(), m_proxies.end(), [proxyId](const ProxyInfo& p) -> bool
		{
			return proxyId == p.proxyId;
		});

	if (result == m_proxies.end())
	{
		return -1;
	}

	return KCAST(int32, std::distance(m_proxies.begin(), result));
}

float KCollisionOverlord::RaycastCB::RayCastCallback(const b2RayCastInput& input, int id)
{
	if (castingID != -1)
	{
		if (id == castingID)
		{
			return 1.0f;
		}
	}
	KPRINTF("RAYCAST CB CALLED\n");

	return 0.0f;
}
