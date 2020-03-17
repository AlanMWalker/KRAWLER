#ifndef KQUADTREE
#define KQUADTREE

#include <vector>
#include <stack> 

#include "Krawler.h"
#include "KEntity.h"

namespace Krawler
{
	enum LeavesIdentifier : int32
	{
		noLeaf = -1,
		northWest,
		northEast,
		southWest,
		southEast,
		leavesIdentifierCount
	};

	class KQuadtree
	{
	public:

		KQuadtree(int level, const Rectf& bounds)
			: m_level(level), m_boundary(bounds), m_bHasSubdivided(false), m_leaves{ nullptr }
		{
		}

		~KQuadtree()
		{

		}

		bool insert(KEntity* p);
		KDEPRECATED_FUNC(std::vector<KEntity*>& queryEntitiy)(KEntity* pEntity);
		void getPossibleCollisions(KEntity* pEntity, std::stack<KEntity*>& entityStack);
		void clear();

	private:

		void subdivide();
		LeavesIdentifier getLeafEnum(KEntity* pEntity) const;
		std::stack<LeavesIdentifier> getLeavesEnum(KEntity* pEntity) const;

		constexpr static int MAX_ENTITIES = 10;
		constexpr static int MAX_NUM_LEVELS = CHUNK_POOL_SIZE / MAX_ENTITIES;

		KQuadtree* m_leaves[LeavesIdentifier::leavesIdentifierCount];

		Rectf m_boundary;

		std::vector<KEntity*> m_nodeVector;
		std::vector<KEntity*> m_queriedPointList;

		std::stack<KEntity*> m_queriedEntityStack;

		int m_level;
		bool m_bHasSubdivided;

	};
}

#endif 
