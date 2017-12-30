#include "KScene.h"

#include "Components\KCTransform.h"
#include "Components\KCBoxCollider.h"

using namespace Krawler;
using namespace Krawler::Components;

//-- KQUADTREE--\\ 

bool KQuadTree::insert(KEntity* pEntity)
{
	auto pTrans = pEntity->getComponent<KCTransform>();

	if (!m_boundary.contains(pTrans->getPosition()))
	{
		return false;
	}

	if ((signed)m_points.size() < MAX_ENTITIES)
	{
		m_points.push_back(pEntity);
		return true;
	}

	if (!m_bHasSubdivided)
	{
		//if (m_level + 1 > MAX_NUM_LEVELS)
		//	return false;
		subdivide();
		m_bHasSubdivided = true;
	}

	for (KQuadTree* pLeaf : m_leaves)
	{
		assert(pLeaf);
		if (pLeaf->insert(pEntity))
		{
			return true;
		}
	}

	return false;
}

std::vector<KEntity*> KQuadTree::queryEntitiy(KEntity* p)
{
	std::vector<KEntity*> pointList;
	auto pTransform = p->getComponent<KCTransform>();

	if (!m_boundary.contains(pTransform->getPosition()))
		return pointList;

	for (auto& point : m_points)
	{
		pointList.push_back(point);
	}

	if (m_bHasSubdivided)
	{
		//TODO Optimise constant copying of vector so query point is less heavy weight
		auto queried = m_leaves[northWest]->queryEntitiy(p);
		pointList.insert(pointList.end(), queried.begin(), queried.end());
		queried = m_leaves[northEast]->queryEntitiy(p);
		pointList.insert(pointList.end(), queried.begin(), queried.end());

		queried = m_leaves[southWest]->queryEntitiy(p);
		pointList.insert(pointList.end(), queried.begin(), queried.end());

		queried = m_leaves[southEast]->queryEntitiy(p);
		pointList.insert(pointList.end(), queried.begin(), queried.end());
	}
	return pointList;
}

void KQuadTree::clear()
{
	if (m_bHasSubdivided)
	{
		for (KQuadTree* pLeaf : m_leaves)
		{
			pLeaf->clear();
			delete pLeaf;
			pLeaf = nullptr;
		}
	}
	m_points.clear();
	m_bHasSubdivided = false;
}

void KQuadTree::subdivide()
{
	if (m_bHasSubdivided)
	{
		return;
	}

	const Point halfBounds(m_boundary.width / 2.0f, m_boundary.height / 2.0f);

	m_leaves[northWest] = new KQuadTree(m_level + 1, sf::FloatRect(Point(m_boundary.left, m_boundary.top), halfBounds));
	m_leaves[northEast] = new KQuadTree(m_level + 1, sf::FloatRect(Point(m_boundary.left + halfBounds.x, m_boundary.top), halfBounds));
	m_leaves[southWest] = new KQuadTree(m_level + 1, sf::FloatRect(Point(m_boundary.left, m_boundary.top + halfBounds.y), halfBounds));
	m_leaves[southEast] = new KQuadTree(m_level + 1, sf::FloatRect(Point(m_boundary.left + halfBounds.x, m_boundary.top + halfBounds.y), halfBounds));
}

// -- KSCENE -- \\

KScene::KScene(std::wstring & sceneName, const Rectf& sceneBounds)
	: m_sceneName(sceneName), m_qtree(0, sceneBounds), m_entitiesInUse(0)
{

}

KInitStatus KScene::initScene()
{
	for (auto& entity : m_entities)
	{
		KINIT_CHECK(entity.init()); // init components
		if (status != KInitStatus::Success) // if failes early out with an error code
		{
			return status;
		}
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
	m_qtree.clear();
	for (uint32 i = 0; i < m_entitiesInUse; ++i)
	{
		m_entities[i].tick(); // tick all components
		m_qtree.insert(m_entities); // insert entity into quadtree before handling box colliders
	}

	// handle box colliders here
	for (uint32 i = 0; i < m_entitiesInUse; ++i)
	{
		auto pCollider = m_entities[i].getComponent<KCBoxCollider>();
		if (!pCollider) // if this entity doesn't have a box collider continue
		{
			continue;
		}
		auto colliderList = m_qtree.queryEntitiy(&m_entities[i]); // query the quadtree for a list of entities 
																  // near the current one that we can query for collisions
		for (auto& pEntity : colliderList) //iterate over all possible entities in this list
		{
			if (pEntity == &m_entities[i]) //if they are the same entity continue
				continue;
			KCBoxCollider* possibleHitCollider = pEntity->getComponent<KCBoxCollider>();
			if (!possibleHitCollider)// if no box collider is found, continue to next collider
			{
				continue;
			}

			bool result = pCollider->checkIntersects(possibleHitCollider); // check for two intersect
			if (result)
			{
				//handle collision callbacks
				possibleHitCollider->collisionCallback(&m_entities[i]);
				pCollider->collisionCallback(pEntity);
			}
		}
	}
}

void Krawler::KScene::fixedTick()
{
	for (uint32 i = 0; i < m_entitiesInUse; ++i)
	{
		m_entities[i].fixedTick();
	}
	//tick physics scene
}

void KScene::onEnterScene()
{
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
	if (m_entitiesInUse + 1 > MAX_NUMBER_OF_ENTITIES)
	{
		return nullptr;
	}
	m_entities[m_entitiesInUse].setIsInUse(true);
	return &m_entities[m_entitiesInUse++];
}

KEntity* KScene::addEntitiesToScene(uint32 number, int32 & numberAllocated)
{
	if (m_entitiesInUse + number <= MAX_NUMBER_OF_ENTITIES) //if there is enough entities in the scene
	{
		numberAllocated = number; // if the number of entities
		KEntity* const pEntity = &m_entities[m_entitiesInUse];
		m_entitiesInUse += number;
		for (uint32 i = 0; i < number; ++i)
		{
			pEntity[i].setIsInUse(true);
		}
		return pEntity;
	}
	return nullptr;
}

KRAWLER_API KEntity * Krawler::KScene::findEntityByTag(const std::wstring & tag)
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
		if (status != Success)
		{
			return status;
		}
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
	static bool bIsFirstTick = true;

	if (bIsFirstTick)
	{
		m_pCurrentScene->onEnterScene();
		bIsFirstTick = false;
	}
	m_pCurrentScene->tick();
}

void KSceneDirector::fixedTickActiveScene()
{
	m_pCurrentScene->tick();
}

void KSceneDirector::setCurrentScene(std::wstring sceneName)
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
		return EXIT_FAILURE;
	}

	m_scenes.push_back(pScene);
	return EXIT_SUCCESS;
}

int32 KSceneDirector::removeScene(KScene * pScene)
{
	auto findResult = std::find(m_scenes.begin(), m_scenes.end(), pScene);

	if (findResult == m_scenes.end())
	{
		return EXIT_FAILURE;
	}

	m_scenes.erase(findResult);
	return EXIT_SUCCESS;
}
