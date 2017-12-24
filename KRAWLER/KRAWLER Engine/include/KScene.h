#ifndef KSCENE_H
#define KSCENE_H

#include "Krawler.h"
#include "KEntity.h"


#include <vector>
#include <list>

namespace Krawler
{
	const int32 MAX_NUMBER_OF_ENTITIES{ 1200 };
	enum LeavesIdentifier : int32
	{
		northWest,
		northEast,
		southWest,
		southEast,
		leavesIdentifierCount
	};

	class KQuadTree
	{
	public:

		KQuadTree(int level, const sf::FloatRect& bounds)
			: m_level(level), m_boundary(bounds), m_bHasSubdivided(false), m_leaves{ nullptr }
		{
		}

		~KQuadTree()
		{

		}

		bool insert(KEntity* p);
		std::vector<KEntity*> queryEntitiy(KEntity* pEntity);
		void clear();

	private:

		void subdivide();

		const int MAX_ENTITIES = 10;
		//const int MAX_NUM_LEVELS = 12;

		KQuadTree* m_leaves[LeavesIdentifier::leavesIdentifierCount];

		Rectf m_boundary;

		std::list<KEntity*> m_points;

		int m_level;
		bool m_bHasSubdivided;
	};

	class KScene
	{
	public:

		KRAWLER_API KScene(std::wstring& sceneName, const Rectf& sceneBounds);
		KRAWLER_API ~KScene() = default;

		//override but call base impl before returning from derived 
		KRAWLER_API virtual KInitStatus initScene(); //init components

		//override but call base impl before returning from derived 
		KRAWLER_API void cleanUpScene();

		//override but call base impl before returning from derived 
		KRAWLER_API virtual void tick();

		//override but call base impl before returning from derived 
		KRAWLER_API virtual void fixedTick();

		//void onEnterScene();
		//void onExitScene();
		KRAWLER_API const std::wstring& getSceneName() const { return m_sceneName; }

		//nullptr if none available or failed
		KRAWLER_API KEntity* addEntityToScene();

		//Will aim to always give a contiguous block. Nullptr if none available or failed
		KRAWLER_API KEntity* addEntitiesToScene(uint32 number, int32& numberAllocated);

		KRAWLER_API KEntity* findEntityByTag(const std::wstring& tag);

		KRAWLER_API uint32 getNumbrOfEntitiesAllocated() const { return m_entitiesInUse; }

	private:

		std::vector<KEntity*> m_renderQueue;

		KEntity m_entities[MAX_NUMBER_OF_ENTITIES];

		std::wstring m_sceneName;
		KQuadTree m_qtree;

		uint32 m_entitiesInUse;
	};

	class KSceneDirector
	{
	public:

	private:

	};
}
#endif
