#include "Utilities\KQuadtree.h"
#include "Components\KCTransform.h"

#include "Components\KCColliderBase.h"

using namespace Krawler;
using namespace Components;

using namespace std;

bool KQuadtree::insert(KEntity* pEntity)
{
	auto pTrans = pEntity->getComponent<KCTransform>();
	auto pCollider = pEntity->getComponent<KCColliderBase>();

	if (!pCollider) // if there's no collider then return false for not added
	{
		return false;
	}

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

vector<KEntity*>& KQuadtree::queryEntitiy(KEntity* p)
{
	m_queriedPointList.clear();
	KCColliderBase* pColliderBase = p->getComponent<KCColliderBase>();
	const Rectf& boundingBox = pColliderBase->getBoundingBox();

	if (!m_boundary.intersects(boundingBox) || m_points.size() == 0)
	{
		return m_queriedPointList;
	}

	if (m_bHasSubdivided)
	{
		const LeavesIdentifier containingLeaf = getLeafEnum(p);
		if (containingLeaf != noLeaf)
		{
			vector<KEntity*>& queried = m_leaves[containingLeaf]->queryEntitiy(p);
			for (auto& pEntity : queried)
			{
				const Rectf& queriedEntityBoundingBox = pEntity->getComponent<KCColliderBase>()->getBoundingBox();
				if (queriedEntityBoundingBox.intersects(boundingBox))
				{
					m_queriedPointList.push_back(pEntity);
				}
			}
		}
	}
	else
	{
		m_queriedPointList = m_points;
	}

	return m_queriedPointList;
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

	for (auto& pEntity : m_points)
	{
		const auto leafEnum = getLeafEnum(pEntity);
		if (leafEnum == noLeaf)
		{
			continue;
		}
		m_leaves[leafEnum]->insert(pEntity);

	}
	m_points.clear();
}

LeavesIdentifier Krawler::KQuadtree::getLeafEnum(KEntity * pEntity) const
{
	const float halfWidth = m_boundary.width / 2.0f, halfHeight = m_boundary.height / 2.0f;
	const Rectf nw(m_boundary.left, m_boundary.top, halfWidth, halfHeight);
	const Rectf ne(m_boundary.left + halfWidth, m_boundary.top, halfWidth, halfWidth);
	const Rectf se(m_boundary.left + halfWidth, m_boundary.top + halfHeight, halfWidth, halfHeight);
	const Rectf sw(m_boundary.left, m_boundary.top + halfHeight, halfWidth, halfHeight);

	const Vec2f pos = pEntity->getComponent<KCTransform>()->getPosition();

	if (nw.contains(pos))
	{
		return northWest;
	}

	if (ne.contains(pos))
	{
		return northEast;
	}

	if (se.contains(pos))
	{
		return southEast;
	}

	if (sw.contains(pos))
	{
		return southWest;
	}

	return LeavesIdentifier::noLeaf;
}
