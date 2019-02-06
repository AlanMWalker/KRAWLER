#include "KApplication.h"

#include "Physics\KPhysicsWorld.h"
#include "Components\KCBoxCollider.h"

using namespace Krawler;
using namespace Krawler::Physics;
using namespace Krawler::Components;
using namespace Krawler::Maths;

KPhysicsWorld::KPhysicsWorld(KPhysicsWorldProperties properties)
	: m_properties(properties), m_dynamicTree(nullptr), m_staticTree(nullptr)
{

}

void KPhysicsWorld::fixedTick()
{
	const float dt = KApplication::getApp()->getPhysicsDelta();
	assembleEntityList();

	for (auto& collisionData : m_collisionsToCheck)
	{
		resolveCollisions(collisionData);
		correctPosition(collisionData);
	}
	m_collisionsToCheck.clear();
}

void KPhysicsWorld::setQuadtrees(KQuadtree * pDynamicTree, KQuadtree* pStaticTree)
{
	KCHECK(pDynamicTree);
	if (!pDynamicTree)
	{
		return;
	}
	m_dynamicTree = pDynamicTree;
	m_staticTree = pStaticTree;
}

void KPhysicsWorld::assembleEntityList()
{
	m_physicsEntities.clear();

	auto& rSceneDirector = KApplication::getApp()->getSceneDirector();
	KScene* pScene = rSceneDirector.getCurrentScene();

	auto pEntities = pScene->getEntityList();
	KCHECK(pEntities);

	for (int32 i = 0; i < (signed)CHUNK_POOL_SIZE; ++i)
	{
		if (!pEntities[i].allocated)
		{
			continue;
		}

		auto& entity = pEntities[i].entity;
		if (!entity.isEntityInUse())
		{//contine if not in use
			continue;
		}
		KCPhysicsBody* pPhysicsBody = entity.getComponent<Components::KCPhysicsBody>();

		//Continue if doesn't have physics body
		if (!pPhysicsBody)
		{
			continue;
		}
		m_physicsEntities.push_back(&entity);
	}

	for (KEntity* pEntity : m_physicsEntities)
	{
		auto colliderBase = pEntity->getComponent<KCColliderBase>();

		if (!colliderBase)
			continue;

		if (!colliderBase->isCallbackSubscribed(&m_collisionCallback))
		{
			colliderBase->subscribeCollisionCallback(&m_collisionCallback);
		}
	}
}

void KPhysicsWorld::resolveCollisions(const KCollisionDetectionData & collData)
{
	KCPhysicsBody* bodyA = collData.entityA->getComponent<KCPhysicsBody>();
	KCPhysicsBody* bodyB = collData.entityB->getComponent<KCPhysicsBody>();

	KPhysicsBodyProperties& propertiesA = *bodyA->getPhysicsBodyProperties();
	KPhysicsBodyProperties& propertiesB = *bodyB->getPhysicsBodyProperties();

	float e = Max(propertiesB.restitution, propertiesA.restitution);
	Vec2f relativeVelocity = bodyB->getVelocity() - bodyA->getVelocity();
	float velocityAlongNormal = DotProduct(relativeVelocity, collData.collisionNormal);

	float j = -(1 + e) * velocityAlongNormal;

	j /= (propertiesA.invMass + propertiesB.invMass);

	Vec2f impulse = j * collData.collisionNormal;

	bodyA->setVelocity(bodyA->getVelocity() - (impulse * propertiesA.invMass));
	bodyB->setVelocity(bodyB->getVelocity() + (impulse * propertiesB.invMass));

	/* Friction Application */
	relativeVelocity = { bodyB->getVelocity() - bodyA->getVelocity() };

	//get the tangent vector
	Vec2f tangent = relativeVelocity - (DotProduct(relativeVelocity, collData.collisionNormal) * collData.collisionNormal);
	tangent = Normalise(tangent);

	float jt = -DotProduct(relativeVelocity, tangent);
	jt = jt / (propertiesA.invMass + propertiesA.invMass);

	const float mu = GetLength(Vec2f(propertiesA.staticFriction, propertiesB.staticFriction)); //static coefficient

	const float dMu = GetLength(Vec2f(propertiesA.dynamicFriction, propertiesB.dynamicFriction)); //dynamic coefficient

	Vec2f frictionImpulse;
	if (abs(jt) < j * mu)
	{
		frictionImpulse = jt * tangent;
	}
	else
	{
		frictionImpulse = -j * tangent * dMu;
	}


	bodyA->setVelocity(bodyA->getVelocity() - (frictionImpulse * propertiesA.invMass));
	bodyB->setVelocity(bodyB->getVelocity() + (frictionImpulse * propertiesB.invMass));
}

void KPhysicsWorld::correctPosition(const KCollisionDetectionData & collData)
{
	KCTransform* const transformA = collData.entityA->getComponent<KCTransform>();
	KCTransform* const transformB = collData.entityB->getComponent<KCTransform>();

	KPhysicsBodyProperties& propertiesA = *collData.entityA->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties();
	KPhysicsBodyProperties& propertiesB = *collData.entityB->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties();


	Vec2f correction = Max(collData.penetration - m_properties.correctionThreshold, 0.0f) / (propertiesA.invMass + propertiesB.invMass) * m_properties.correctionPercentage* collData.collisionNormal;
	transformA->move(-propertiesA.invMass * correction);
	transformB->move(propertiesB.invMass * correction);

}

void KPhysicsWorld::addToCollisionsList(const KCollisionDetectionData & collData)
{
	KCPhysicsBody* pBodyA = collData.entityA->getComponent<KCPhysicsBody>();
	KCPhysicsBody* pBodyB = collData.entityB->getComponent<KCPhysicsBody>();

	if (!pBodyA || !pBodyB)
	{
		return;
	}
	const auto checkCollisionDataIsntInList = [&collData](const KCollisionDetectionData& vectorEntry) -> bool
	{
		return collData == vectorEntry;
	};
	auto findResult = std::find_if(m_collisionsToCheck.begin(), m_collisionsToCheck.end(), checkCollisionDataIsntInList);

	if (findResult != m_collisionsToCheck.end())
	{
		return;
	}

	m_collisionsToCheck.push_back(collData);
}
