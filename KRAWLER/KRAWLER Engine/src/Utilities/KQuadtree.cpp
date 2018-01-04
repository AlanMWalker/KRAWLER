#include "Utilities\KQuadtree.h"
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Components;

bool KQuadtree::insert(KEntity* pEntity)
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

	for (KQuadtree* pLeaf : m_leaves)
	{
		assert(pLeaf);
		if (pLeaf->insert(pEntity))
		{
			return true;
		}
	}

	return false;
}

std::vector<KEntity*> KQuadtree::queryEntitiy(KEntity* p)
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

void KQuadtree::clear()
{
	if (m_bHasSubdivided)
	{
		for (KQuadtree* pLeaf : m_leaves)
		{
			pLeaf->clear();
			delete pLeaf;
			pLeaf = nullptr;
		}
	}
	m_points.clear();
	m_bHasSubdivided = false;
}

void KQuadtree::subdivide()
{
	if (m_bHasSubdivided || m_level + 1 >= MAX_NUM_LEVELS)
	{
		return;
	}

	const Point halfBounds(m_boundary.width / 2.0f, m_boundary.height / 2.0f);

	m_leaves[northWest] = new KQuadtree(m_level + 1, sf::FloatRect(Point(m_boundary.left, m_boundary.top), halfBounds));
	m_leaves[northEast] = new KQuadtree(m_level + 1, sf::FloatRect(Point(m_boundary.left + halfBounds.x, m_boundary.top), halfBounds));
	m_leaves[southWest] = new KQuadtree(m_level + 1, sf::FloatRect(Point(m_boundary.left, m_boundary.top + halfBounds.y), halfBounds));
	m_leaves[southEast] = new KQuadtree(m_level + 1, sf::FloatRect(Point(m_boundary.left + halfBounds.x, m_boundary.top + halfBounds.y), halfBounds));
}
