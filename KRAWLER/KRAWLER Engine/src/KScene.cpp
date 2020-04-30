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

KScene::KScene(const std::wstring& sceneName, const Rectf& sceneBounds)
	: m_sceneName(sceneName), m_numberOfAllocatedChunks(0)
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
	return KInitStatus::Success;
}

void KScene::cleanUpScene()
{
	for (auto& chunk : m_entityChunks)
	{
		chunk.entity.cleanUp();
	}
}

void KScene::tick()
{
	KApplication::getMutexInstance().lock();
	if (!m_bHasTickedOnce)
	{
		m_bHasTickedOnce = true;
	}

	for (uint32 i = 0; i < CHUNK_POOL_SIZE; ++i)
	{
		if (!m_entityChunks[i].allocated) // if not allocated to the scene, ignore
		{
			continue;
		}
		if (!m_entityChunks[i].entity.isEntityActive())
		{
			continue;
		}

		m_entityChunks[i].entity.tick(); // tick all components
	}
	KApplication::getMutexInstance().unlock();
}

void KScene::fixedTick()
{
	static std::stack<KEntity*> colliderStack;
	static vector<pair<KEntity*, KEntity*>> alreadyCheckedCollisionPairs(500);

	//TODO remove mutex lock 
	KApplication::getMutexInstance().lock();
	for (uint32 i = 0; i < CHUNK_POOL_SIZE; ++i)
	{
		if (!m_entityChunks[i].allocated)
		{
			continue;
		}

		if (!m_entityChunks[i].entity.isEntityActive())
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

KEntity* KScene::addEntitiesToScene(uint32 number, int32& numberAllocated)
{
	if (m_numberOfAllocatedChunks + number <= CHUNK_POOL_SIZE) //if there is enough entities in the scene
	{
		numberAllocated = number; // if the number of entities
		KEntity* const pEntity = &(m_entityChunks[m_numberOfAllocatedChunks].entity);
		m_numberOfAllocatedChunks += number;
		for (uint32 i = 0; i < number; ++i)
		{
			pEntity[i].setActive(true);
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
			it->entity.setActive(true);
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

KEntity* KScene::findEntityByTag(const std::wstring& tag)
{
	auto find = std::find_if(std::begin(m_entityChunks), std::end(m_entityChunks), [&tag](const KAllocatableChunk& chunk) -> bool
		{
			if (chunk.entity.getTag() == tag)
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

std::vector<KEntity*> KScene::getAllocatedEntityList()
{
	std::vector<KEntity*> out;
	for (auto& c : m_entityChunks)
	{
		if (!c.allocated)
		{
			continue;
		}
		out.push_back(&c.entity);
	}
	return out;
}

//private
KEntity* KScene::getAllocatableEntity()
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
	findResult->entity.setActive(true);
	return &findResult->entity;
}

int32 KScene::getFreeChunkTotal() const
{
	int32 count = 0;
	for (auto& chunk : m_entityChunks)
	{
		if (!chunk.allocated)
			++count;
	}

	return count;
}

//-- KSCENEDIRECTOR -- \\

KSceneDirector::KSceneDirector()
	: m_pCurrentScene(nullptr), m_pNextScene(nullptr)
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
	// If we're transitioning between scenes
	// then close the last scene and exit
	// the next tick after this will invoke 
	// onEnterScene for the new scene
	if (m_bIsChangingScene)
	{
		KCHECK(m_pNextScene);
		m_pCurrentScene->onExitScene();
		m_pCurrentScene = m_pNextScene;
		m_bIsChangingScene = false;
		m_pNextScene = nullptr;
		GET_APP()->getOverlord().triggerSceneCleanup();
		return;
	}

	// If this is the first tick for a scene
	// then we'll call onEnterScene() routine
	if (!m_pCurrentScene->hasSceneTickedOnce())
	{
		m_pCurrentScene->onEnterScene();
		GET_APP()->getOverlord().triggerSceneConstruct();
	}

	m_pCurrentScene->tick();
}

void KSceneDirector::fixedTickActiveScene()
{
	m_pCurrentScene->fixedTick();
}

void KSceneDirector::setStartScene(const std::wstring& sceneName)
{
	auto const pScene = findSceneByName(sceneName);
	if (!pScene)
	{
		return;
	}

	m_pCurrentScene = pScene;
}

void KSceneDirector::transitionToScene(const std::wstring& sceneName)
{
	auto const pScene = findSceneByName(sceneName);
	if (!pScene)
	{
		return;
	}
	m_bIsChangingScene = true;
	m_pNextScene = pScene;
}

int32 KSceneDirector::addScene(KScene* pScene)
{
	KCHECK(pScene);
	if (!pScene)
	{
		return EXIT_FAILURE;
	}

	m_scenes.push_back(pScene);
	return EXIT_SUCCESS;
}

int32 KSceneDirector::removeScene(KScene* pScene)
{
	auto findResult = std::find(m_scenes.begin(), m_scenes.end(), pScene);

	if (findResult == m_scenes.end())
	{
		return EXIT_FAILURE;
	}

	m_scenes.erase(findResult);
	return EXIT_SUCCESS;
}

KScene* KSceneDirector::findSceneByName(const std::wstring& name) const
{
	auto findResult = std::find_if(m_scenes.begin(), m_scenes.end(), [&name](KScene* pScene) -> bool
		{
			return pScene->getSceneName() == name;
		});

	if (findResult == m_scenes.end())
	{
		return nullptr;
	}

	return *findResult;
}
