#ifndef KQUADTREE
#define KQUADTREE

#include <vector>
#include <list> 

#include "Krawler.h"
#include "KEntity.h"

namespace Krawler
{
	enum LeavesIdentifier : int32
	{
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
		std::vector<KEntity*> queryEntitiy(KEntity* pEntity);
		void clear();

	private:

		void subdivide();

		const int MAX_ENTITIES = 200;
		const int MAX_NUM_LEVELS = 5;

		KQuadtree* m_leaves[LeavesIdentifier::leavesIdentifierCount];

		Rectf m_boundary;

		std::list<KEntity*> m_points;

		int m_level;
		bool m_bHasSubdivided;

	};
}
#endif 
