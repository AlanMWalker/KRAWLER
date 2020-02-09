#include "KScene.h"
#include "KApplication.h"

#include "Components\KCTransform.h"
#include "Components\KCColliderBase.h"

#include "Components\KCPhysicsBody.h"
#include "Components\KCTileMap.h"
#include "Utilities\KDebug.h"
#include "Components/KCImgui.h"

using namespace Krawler;
using namespace Krawler::Components;

using namespace std;

// -- KSCENE -- \\

KScene::KScene(const std::wstring & sceneName, const Rectf& sceneBounds)
	: m_sceneName(sceneName), m_dynamicQTree(0, sceneBounds), m_numberOfAllocatedChunks(0), m_staticQTree(4, sceneBounds)
{
	
}

KInitStatus KScene::initScene()
{
	
	
	//First initialisation pass => Initialise all entitys, and setup all components
	for (auto& chunk : m_entityChunks)
	{
		KINIT_CHECK(chunk.entity.init()); // init components
		//TODO Move to onenter
		chunk.entity.getComponent<KCTransform>()->tick(); //tick transforms incase of transforms were applied during init of components
	}
	//Second initialisation pass => put all static elements in static quadtree and build collider list
	for (auto& chunk : m_entityChunks)
	{
		if (chunk.entity.getInteractivity() == Static)
		{
			m_staticQTree.insert(&chunk.entity);
		}
		auto pCollider = chunk.entity.getComponent<KCColliderBase>();
		if (pCollider)
		{
			m_initCachedColliders.push_back(pCollider);
		}
	}


	return KInitStatus::Success;
}

void Krawler::KScene::cleanUpScene()
{
	for (auto& chunk : m_entityChunks)
	{
		chunk.entity.cleanUp();
	}
	m_dynamicQTree.clear();
}

void Krawler::KScene::tick()
{
	KApplication::getMutexInstance().lock();
	if (!m_bHasTickedOnce)
	{
		m_bHasTickedOnce = true;
	}
	m_dynamicQTree.clear();

	for (uint32 i = 0; i < CHUNK_POOL_SIZE; ++i)
	{
		if (!m_entityChunks[i].allocated) // if not allocated to the scene, ignore
		{
			continue;
		}
		if (!m_entityChunks[i].entity.isEntityInUse())
		{
			continue;
		}

		m_entityChunks[i].entity.tick(); // tick all components

		if (m_entityChunks[i].entity.getInteractivity() == EntitySceneInteractivity::Dynamic)
		{
			m_dynamicQTree.insert(&m_entityChunks[i].entity); // insert entity into quadtree before handling box colliders
		}
	}
	KApplication::getMutexInstance().unlock();
}

void Krawler::KScene::fixedTick()
{
	static std::stack<KEntity*> colliderStack;
	static vector<pair<KEntity*, KEntity*>> alreadyCheckedCollisionPairs(500);

	//TODO remove mutex lock 
	KApplication::getMutexInstance().lock();

	//TODO remove this form of querying dynamic quadtree
	// handle colliders here
	KCColliderBase* pCollider = nullptr;
	KCColliderBase* possibleHitCollider = nullptr;

	for (uint32 i = 0; i < m_initCachedColliders.size(); ++i)
	{

		KEntity* pEntity = m_initCachedColliders[i]->getEntity();
		KCHECK(pEntity);
		if (!pEntity->isEntityInUse())
		{//if entity isn't in use
			continue;
		}

		if (pEntity->getInteractivity() == Static)
			continue;

		pCollider = m_initCachedColliders[i]; // cache colliders after initialisation
		KCHECK(pCollider);

		m_dynamicQTree.getPossibleCollisions(pEntity, colliderStack);

		m_staticQTree.getPossibleCollisions(pEntity, colliderStack);
		const uint64 stackSize = colliderStack.size();
		int32 alreadyCheckedIndex = 0;

		while (!colliderStack.empty())
		{
			KEntity* pEntityTestedAgainst = colliderStack.top();

			//check pairs
			KCHECK(pEntityTestedAgainst);

			if (pEntityTestedAgainst == pEntity) //if they are the same entity continue
			{
				continue;
			}

			const pair<KEntity*, KEntity*> pairA(pEntity, pEntityTestedAgainst);

			for (int i = 0; i < m_initCachedColliders.size(); ++i)
			{
				if (m_initCachedColliders[i]->getEntity() == pEntityTestedAgainst)
				{
					possibleHitCollider = m_initCachedColliders[i];
				}
			}

			KCHECK(possibleHitCollider);

			if (!possibleHitCollider)// if no box collider is found, continue to next collider
			{
				colliderStack.pop();
				continue;
			}
			const KCColliderFilteringData& filterA = pCollider->getCollisionFilteringData();
			const KCColliderFilteringData& filterB = possibleHitCollider->getCollisionFilteringData();

			// if collision filters tested against collision masks for entity a & b exclude them from being allowed
			// to collide with one another, then continue onto the next pair.

			if ((filterA.collisionFilter & filterB.collisionMask) == 0 || (filterB.collisionFilter & filterA.collisionMask) == 0)
			{
				colliderStack.pop();
				continue;
			}

			KCollisionDetectionData data;
			data.entityA = pairA.first;
			data.entityB = pairA.second;

			alreadyCheckedCollisionPairs[alreadyCheckedIndex] = pairA;
			++alreadyCheckedIndex;

			const bool result = CollisionLookupTable[static_cast<uint32>(pCollider->getColliderType())][static_cast<uint32>(possibleHitCollider->getColliderType())](data);

			if (result)
			{
				//handle collision callbacks
				possibleHitCollider->collisionCallback(data);
				pCollider->collisionCallback(data);
			}

			colliderStack.pop();

		}

	}


	for (uint32 i = 0; i < CHUNK_POOL_SIZE; ++i)
	{
		if (!m_entityChunks[i].allocated)
		{
			continue;
		}

		if (!m_entityChunks[i].entity.isEntityInUse())
		{
			continue;
		}
		m_entityChunks[i].entity.fixedTick();
	}

	KApplication::getMutexInstance().unlock();
}

void KScene::onEnterScene()
{
	KApplication::getMutexInstance().lock();

	KApplication::getApp()->getPhysicsWorld()->setQuadtrees(&m_dynamicQTree, &m_staticQTree);
	for (auto& entityChunk : m_entityChunks)
	{
		if (!entityChunk.allocated)
		{
			continue;
		}

		entityChunk.entity.onEnterScene();
	}

	KApplication::getMutexInstance().unlock();
}

void KScene::onExitScene()
{
	for (auto& entityChunk : m_entityChunks)
	{
		if (!entityChunk.allocated)
		{
			continue;
		}
		entityChunk.entity.onExitScene();
	}
}

KEntity* KScene::addEntityToScene()
{
	KEntity* const pEntity = getAllocatableEntity();
	if (!pEntity)
	{
		return nullptr;
	}

	++m_numberOfAllocatedChunks;
	return pEntity;
}

KEntity* KScene::addEntitiesToScene(uint32 number, int32 & numberAllocated)
{
	if (m_numberOfAllocatedChunks + number <= CHUNK_POOL_SIZE) //if there is enough entities in the scene
	{
		numberAllocated = number; // if the number of entities
		KEntity* const pEntity = &(m_entityChunks[m_numberOfAllocatedChunks].entity);
		m_numberOfAllocatedChunks += number;
		for (uint32 i = 0; i < number; ++i)
		{
			pEntity[i].setIsInUse(true);
		}
		return pEntity;
	}
	return nullptr;
}

KRAWLER_API bool KScene::addMultipleEntitiesToScene(uint32 numberToAllocate, vector<KEntity*>& entityVec)
{
	const int32 total = getFreeChunkTotal();
	//if there isn't enough available memory, do not allocate
	if (total < (signed)numberToAllocate)
	{
		return false;
	}
	entityVec.resize(numberToAllocate);
	int32 count = 0;

	for (auto it = std::begin(m_entityChunks); it != std::end(m_entityChunks); ++it)
	{
		if (count >= (signed)numberToAllocate)
		{
			break;
		}
		if (!it->allocated)
		{
			it->allocated = true;
			it->entity.setIsInUse(true);
			entityVec[count] = &it->entity;
			++count;
		}
	}

	return true;
}

void KScene::removeEntityFromScene(KEntity* pEntityToRemove)
{
	KCHECK(pEntityToRemove);
	if (!pEntityToRemove)
	{
		return;
	}

	for (auto it = std::begin(m_entityChunks); it != std::end(m_entityChunks); ++it)
	{
		if (&it->entity == pEntityToRemove)
		{
			pEntityToRemove->cleanUp();
		}
	}
}

KEntity * Krawler::KScene::findEntityByTag(const std::wstring & tag)
{
	auto find = std::find_if(std::begin(m_entityChunks), std::end(m_entityChunks), [&tag](const KAllocatableChunk& chunk) -> bool
	{
		if (chunk.entity.getEntityTag() == tag)
		{
			return true;
		}
		return false;
	});

	if (find == std::end(m_entityChunks))
	{
		return nullptr;
	}
	return &(*find).entity;
}

//private
Krawler::KEntity* KScene::getAllocatableEntity()
{
	auto findResult = std::find_if(std::begin(m_entityChunks), std::end(m_entityChunks), [](KAllocatableChunk& entity) -> bool
	{
		return !entity.allocated;
	});

	if (findResult == std::end(m_entityChunks))
	{
		return nullptr;
	}
	findResult->allocated = true;
	findResult->entity.setIsInUse(true);
	return &findResult->entity;
}

Krawler::int32 Krawler::KScene::getFreeChunkTotal() const
{
	int32 count = 0;
	for (auto& chunk : m_entityChunks)
		if (!chunk.allocated)
			++count;
	return count;
}

//-- KSCENEDIRECTOR -- \\

Krawler::KSceneDirector::KSceneDirector()
	: m_pCurrentScene(nullptr)
{
}

KInitStatus KSceneDirector::initScenes()
{
	if (m_pCurrentScene == nullptr)
		m_pCurrentScene = m_scenes[0];

	for (auto& pScene : m_scenes)
	{
		KINIT_CHECK(pScene->initScene());
	}

	KCHECK(m_scenes.size() > 0); // check we have scenes available

	if (m_scenes.size() == 0)
	{
		KPrintf(KTEXT("Failed to initialise scene director! No scenes added"));
		return KInitStatus::Failure;
	}


	return KInitStatus::Success;
}

void KSceneDirector::cleanupScenes()
{
	for (auto& pScene : m_scenes)
	{
		pScene->cleanUpScene();
		KFREE(pScene);
	}
	m_scenes.clear();
}

void KSceneDirector::tickActiveScene()
{
	if (!m_pCurrentScene->hasSceneTickedOnce())
	{
		m_pCurrentScene->onEnterScene();
	}
	m_pCurrentScene->tick();
}

void KSceneDirector::fixedTickActiveScene()
{
	m_pCurrentScene->fixedTick();
}

void KSceneDirector::setCurrentScene(const std::wstring& sceneName)
{
	auto findResult = std::find_if(m_scenes.begin(), m_scenes.end(), [&sceneName](KScene* pScene) -> bool
	{
		return pScene->getSceneName() == sceneName;
	});
	if (findResult == m_scenes.end())
	{
		return;
	}
	m_pCurrentScene = *findResult;

}

int32 KSceneDirector::addScene(KScene * pScene)
{
	KCHECK(pScene);
	if (!pScene)
	{
		return -EXIT_FAILURE;
	}

	m_scenes.push_back(pScene);
	return EXIT_SUCCESS;
}

int32 KSceneDirector::removeScene(KScene * pScene)
{
	auto findResult = std::find(m_scenes.begin(), m_scenes.end(), pScene);

	if (findResult == m_scenes.end())
	{
		return -EXIT_FAILURE;
	}

	m_scenes.erase(findResult);
	return EXIT_SUCCESS;
}
