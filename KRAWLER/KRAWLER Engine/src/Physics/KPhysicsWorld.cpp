#include "KApplication.h"

#include "Physics\KPhysicsWorld.h"
#include "Components\KCBoxCollider.h"

using namespace Krawler;
using namespace Krawler::Physics;
using namespace Krawler::Components;

KPhysicsWorld::KPhysicsWorld(KPhysicsWorldProperties properties)
	: m_properties(properties), m_pQtree(nullptr)
{

}

void KPhysicsWorld::fixedTick()
{
	const float dt = KApplication::getApp()->getPhysicsDelta();
	assembleEntityList();
	resolveCollisions();

}

void KPhysicsWorld::setQuadtree(KQuadtree * pQtree)
{
	KCHECK(pQtree);
	if (!pQtree)
	{
		return;
	}
	m_pQtree = pQtree;
}

void KPhysicsWorld::assembleEntityList()
{
	m_physicsEntities.clear();

	auto& rSceneDirector = KApplication::getApp()->getSceneDirector();
	KScene* pScene = rSceneDirector.getCurrentScene();

	auto pEntities = pScene->getEntitiyList();
	KCHECK(pEntities);

	for (int32 i = 0; i < (signed) pScene->getNumbrOfEntitiesAllocated(); ++i)
	{
		auto& entity = pEntities[i];
		if (!entity.isEntitiyInUse())
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
}

void KPhysicsWorld::resolveCollisions()
{
}
