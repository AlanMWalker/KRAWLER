#include "KScene.h"
#include "KApplication.h"	 

#include "Components\KCTransform.h"
#include "Components\KCBoxCollider.h"

using namespace Krawler;
using namespace Krawler::Components;

using namespace std;

// -- KSCENE -- \\

KScene::KScene(const std::wstring & sceneName, const Rectf& sceneBounds)
	: m_sceneName(sceneName), m_qtree(0, sceneBounds), m_entitiesAllocated(0)
{

}

KInitStatus KScene::initScene()
{
	for (auto& entity : m_entities)
	{
		KINIT_CHECK(entity.init()); // init components
		//TODO Move to onenter
		entity.getComponent<KCTransform>()->tick(); //tick transforms incase of transforms were applied during init of components
	}
	return KInitStatus::Success;
}

void Krawler::KScene::cleanUpScene()
{
	for (auto& entity : m_entities)
	{
		entity.cleanUp();
	}
	m_qtree.clear();
}

void Krawler::KScene::tick()
{
	m_bHasTickedOnce = true;
	m_qtree.clear();

	for (uint32 i = 0; i < m_entitiesAllocated; ++i)
	{
		if (!m_entities[i].isEntitiyInUse())
			continue;

		m_entities[i].tick(); // tick all components
		m_qtree.insert(&m_entities[i]); // insert entity into quadtree before handling box colliders
	}

	const auto isCollisionPairEqual = [](std::pair<KEntity*, KEntity*>& pairA, std::pair<KEntity*, KEntity*>& pairB) -> bool
	{
		return (pairA.first == pairB.second  && pairA.second == pairB.second) || (pairA.first == pairB.second && pairA.second == pairB.first);
	};

	vector<pair<KEntity*, KEntity*>> alreadyCheckedCollisionPairs;
	// handle box colliders here
	for (uint32 i = 0; i < m_entitiesAllocated; ++i)
	{
		if (!m_entities[i].isEntitiyInUse())
		{//if entity isn't in use
			continue;
		}

		auto pCollider = m_entities[i].getComponent<KCBoxCollider>();

		if (!pCollider) // if this entity doesn't have a box collider continue
		{
			continue;
		}

		auto& colliderList = m_qtree.queryEntitiy(&m_entities[i]); // query the quadtree for a list of entities 
																  // near the current one that we can query for collisions
		for (auto& pEntity : colliderList) //iterate over all possible entities in this list
		{
			//check pairs
			KCHECK(pEntity);

			if (pEntity == &m_entities[i]) //if they are the same entity continue
			{
				continue;
			}
			const pair<KEntity*, KEntity*> pairA(&m_entities[i], pEntity);
			const pair<KEntity*, KEntity*> pairASwapped(pEntity, &m_entities[i]);

			auto isEqualA = std::find(alreadyCheckedCollisionPairs.begin(), alreadyCheckedCollisionPairs.end(), pairA);
			auto isEqualASwapped = std::find(alreadyCheckedCollisionPairs.begin(), alreadyCheckedCollisionPairs.end(), pairASwapped);

			if (isEqualA != alreadyCheckedCollisionPairs.end() || isEqualASwapped != alreadyCheckedCollisionPairs.end())
			{
				continue;
			}

			KCBoxCollider* const possibleHitCollider = pEntity->getComponent<KCBoxCollider>();

			if (!possibleHitCollider)// if no box collider is found, continue to next collider
			{
				continue;
			}
			const bool result = pCollider->checkIntersects(possibleHitCollider); // check for two intersect

			if (result)
			{
				//handle collision callbacks
				possibleHitCollider->collisionCallback(&m_entities[i]);
				pCollider->collisionCallback(pEntity);
			}
		}
	}
	//KPrintf(L"Size - %d \n", alreadyCheckedCollisionPairs.size());
}

void Krawler::KScene::fixedTick()
{
	for (uint32 i = 0; i < m_entitiesAllocated; ++i)
	{
		if (!m_entities[i].isEntitiyInUse())
		{
			continue;
		}
		m_entities[i].fixedTick();
	}
}

void KScene::onEnterScene()
{
	KApplication::getApp()->getPhysicsWorld()->setQuadtree(&m_qtree);
	for (auto& entity : m_entities)
	{
		entity.onEnterScene();
	}
}

void KScene::onExitScene()
{
	for (auto& entity : m_entities)
	{
		entity.onExitScene();
	}
}

KEntity* KScene::addEntityToScene()
{
	if (m_entitiesAllocated + 1 >= MAX_NUMBER_OF_ENTITIES)
	{
		return nullptr;
	}
	m_entities[m_entitiesAllocated].setIsInUse(true);
	return &m_entities[m_entitiesAllocated++];
}

KEntity* KScene::addEntitiesToScene(uint32 number, int32 & numberAllocated)
{
	if (m_entitiesAllocated + number <= MAX_NUMBER_OF_ENTITIES) //if there is enough entities in the scene
	{
		numberAllocated = number; // if the number of entities
		KEntity* const pEntity = &m_entities[m_entitiesAllocated];
		m_entitiesAllocated += number;
		for (uint32 i = 0; i < number; ++i)
		{
			pEntity[i].setIsInUse(true);
		}
		return pEntity;
	}
	return nullptr;
}

KEntity * Krawler::KScene::findEntityByTag(const std::wstring & tag)
{
	auto find = std::find_if(std::begin(m_entities), std::end(m_entities), [&tag](const KEntity& entity) -> bool
	{
		if (entity.getEntityTag() == tag)
		{
			return true;
		}
		return false;
	});

	if (find == std::end(m_entities))
	{
		return nullptr;
	}
	return &(*find);
}

//-- KSCENEDIRECTOR -- \\

Krawler::KSceneDirector::KSceneDirector()
	: m_pCurrentScene(nullptr)
{
}

KInitStatus KSceneDirector::initScenes()
{
	for (auto& pScene : m_scenes)
	{
		KINIT_CHECK(pScene->initScene());
	}

	KCHECK(m_scenes.size() > 0); // check we have scenes available

	if (m_scenes.size() == 0)
	{
		KPrintf(KTEXT("Failed to initialise scene director! No scenes added"));
		return Failure;
	}

	if (m_pCurrentScene == nullptr)
		m_pCurrentScene = m_scenes[0];

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
	if (m_pCurrentScene->hasSceneTickedOnce())
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
