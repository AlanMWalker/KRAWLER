#ifndef KSCENE_H
#define KSCENE_H

#include "Krawler.h"
#include "KEntity.h"

#include "Physics\KPhysicsWorld.h"
#include "Utilities\KQuadtree.h"

#include <vector>
#include <list>


namespace Krawler
{
	struct KAllocatableChunk
	{
		bool allocated = 0;
		KEntity entity;
	};

	struct __declspec(deprecated("Collision module is deprecated!")) KCollisionOverseer
	{
		KQuadtree* pStaticQTree = nullptr;
		KQuadtree* pDynamicQTree = nullptr;
	};

	namespace Components
	{
		class KCColliderBase;
		class KCImgui;
	}

	class KScene
	{
	public:

		KRAWLER_API KScene(const std::wstring& sceneName, const Rectf& sceneBounds);
		KRAWLER_API ~KScene() = default;

		//override but call base impl before returning from derived 
		KRAWLER_API virtual KInitStatus initScene(); //init components

		//override but call base impl before returning from derived 
		KRAWLER_API void cleanUpScene();

		//override but call base impl before returning from derived 
		KRAWLER_API virtual void tick();

		//override but call base impl before returning from derived 
		KRAWLER_API virtual void fixedTick();

		//override but call base impl before returning from derived
		KRAWLER_API void onEnterScene();

		KRAWLER_API void onExitScene();

		KRAWLER_API const std::wstring& getSceneName() const { return m_sceneName; }

		//nullptr if none available or failed
		KRAWLER_API KEntity* addEntityToScene();

		//Will aim to always give a contiguous block. Nullptr if none available or failed
		KRAWLER_API KDEPRECATED_FUNC(KEntity* addEntitiesToScene)(Krawler::uint32 number, Krawler::int32& numberAllocated);

		KRAWLER_API bool addMultipleEntitiesToScene(uint32 numberToAllocate, std::vector<KEntity*>& entityVec);

		KRAWLER_API void removeEntityFromScene(KEntity* pEntityToRemove);

		KRAWLER_API KEntity* findEntityByTag(const std::wstring& tag);

		KRAWLER_API uint32 getNumbrOfEntitiesAllocated() const { return m_numberOfAllocatedChunks; }

		KRAWLER_API std::vector<KEntity*> getAllocatedEntityList();

		KRAWLER_API KAllocatableChunk* getEntityList() { return m_entityChunks; }

		KRAWLER_API KCollisionOverseer getCollisionOverseer() { return KCollisionOverseer{ &m_staticQTree, &m_dynamicQTree }; }

		bool hasSceneTickedOnce() const { return m_bHasTickedOnce; }

	private:

		Krawler::KEntity* getAllocatableEntity();
		// Get the total number of unallocated chunks in the memory pool
		Krawler::int32 getFreeChunkTotal() const;
		bool m_bHasTickedOnce = false;

		const int32 VelocityIterations = 6;
		const int32 PositionIterations = 2;

		KAllocatableChunk m_entityChunks[CHUNK_POOL_SIZE];

		std::wstring m_sceneName;
		std::vector<Components::KCColliderBase*> m_initCachedColliders;
		KQuadtree m_dynamicQTree;
		KQuadtree m_staticQTree;
		uint32 m_numberOfAllocatedChunks;
		Components::KCImgui* m_pImguiComponent = nullptr;
	};

	class KSceneDirector
	{
	public:

		KRAWLER_API KSceneDirector();
		KRAWLER_API ~KSceneDirector() = default;

		KRAWLER_API KInitStatus initScenes();
		KRAWLER_API void cleanupScenes();

		KRAWLER_API void tickActiveScene();
		KRAWLER_API void fixedTickActiveScene();

		//@param Name of scene to start the application on
		KRAWLER_API void setStartScene(const std::wstring& sceneName);

		//@param Name of scene to transition to
		KRAWLER_API void transitionToScene(const std::wstring& sceneName);


		KRAWLER_API int32 addScene(KScene* pScene); //return 0 if added, EXIT_FAILURE if failed
		KRAWLER_API int32 removeScene(KScene* pScene); //return 0 if removed, EXIT_FAILURE if failed

		KRAWLER_API KScene* const getCurrentScene() { return m_pCurrentScene; }

	private:

		KScene* findSceneByName(const std::wstring& name) const;

		std::vector<KScene*> m_scenes;
		KScene* m_pCurrentScene;
		KScene* m_pNextScene;
		bool m_bIsChangingScene = false;
	};
}
#endif
