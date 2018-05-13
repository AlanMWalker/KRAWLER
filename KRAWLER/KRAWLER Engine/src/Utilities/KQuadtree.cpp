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

	if (!pCollider || !pEntity->isEntityInUse()) // if there's no collider then return false for not added
	{
		return false;
	}
	for (auto& p : m_nodeVector)
	{
		if (p == pEntity)
			return false;
	}

	if (!m_boundary.intersects(pCollider->getBoundingBox()))
	{
		return false;
	}

	if ((signed)m_nodeVector.size() < MAX_ENTITIES)
	{
		m_nodeVector.push_back(pEntity);
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

	if (!m_boundary.intersects(boundingBox) || m_nodeVector.size() == 0)
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
		for (auto& pEntity : m_nodeVector)
		{
			if (pEntity == p)
			{// if same object
				continue;
			}
			const Rectf& queriedEntityBoundingBox = pEntity->getComponent<KCColliderBase>()->getBoundingBox();
			if (queriedEntityBoundingBox.intersects(boundingBox))
			{
				m_queriedPointList.push_back(pEntity);
			}
		}
	}

	return m_queriedPointList;
}

void KQuadtree::getPossibleCollisions(KEntity* pEntity, std::stack<KEntity*>& entityStack)
{
	KCColliderBase* pColliderBase = pEntity->getComponent<KCColliderBase>();
	if (!pColliderBase)
	{
		return;
	}
	const Rectf& boundingBox = pColliderBase->getBoundingBox();

	if (!m_boundary.intersects(boundingBox) || m_nodeVector.size() == 0)
	{
		return;
	}

	if (m_bHasSubdivided)
	{
		const LeavesIdentifier containingLeaf = getLeafEnum(pEntity);
		std::stack<LeavesIdentifier> leafStack(getLeavesEnum(pEntity));

		std::stack<KEntity*> queried;
		for (int32 i = 0; i < (signed)leafStack.size(); ++i)
		{
			m_leaves[leafStack.top()]->getPossibleCollisions(pEntity, queried);
			while (!queried.empty())
			{
				KEntity* pNode = queried.top();
				if (pNode == pEntity)
				{
					queried.pop();
					continue;
				}

				entityStack.push(pNode);
				queried.pop();

			}
			leafStack.pop();
		}
	}

	for (auto& pNode : m_nodeVector)
	{
		if (pEntity == pNode)
		{// if same object
			continue;
		}
		entityStack.push(pNode);
	}

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
	m_nodeVector.clear();
	m_bHasSubdivided = false;
}

void KQuadtree::subdivide()
{
	if (m_bHasSubdivided || m_level + 1 >= MAX_NUM_LEVELS)
	{
		return;
	}

	const Vec2f halfBounds(m_boundary.width / 2.0f, m_boundary.height / 2.0f);

	m_leaves[northWest] = new KQuadtree(m_level + 1, sf::FloatRect(Vec2f(m_boundary.left, m_boundary.top), halfBounds));
	m_leaves[northEast] = new KQuadtree(m_level + 1, sf::FloatRect(Vec2f(m_boundary.left + halfBounds.x, m_boundary.top), halfBounds));
	m_leaves[southWest] = new KQuadtree(m_level + 1, sf::FloatRect(Vec2f(m_boundary.left, m_boundary.top + halfBounds.y), halfBounds));
	m_leaves[southEast] = new KQuadtree(m_level + 1, sf::FloatRect(Vec2f(m_boundary.left + halfBounds.x, m_boundary.top + halfBounds.y), halfBounds));

	for (auto& pEntity : m_nodeVector)
	{
		const auto leafEnum = getLeafEnum(pEntity);
		if (leafEnum == noLeaf)
		{
			continue;
		}
		m_leaves[leafEnum]->insert(pEntity);

	}
	m_nodeVector.clear();
}

LeavesIdentifier Krawler::KQuadtree::getLeafEnum(KEntity * pEntity) const
{
	const float halfWidth = m_boundary.width / 2.0f, halfHeight = m_boundary.height / 2.0f;
	const Rectf nw(m_boundary.left, m_boundary.top, halfWidth, halfHeight);
	const Rectf ne(m_boundary.left + halfWidth, m_boundary.top, halfWidth, halfWidth);
	const Rectf se(m_boundary.left + halfWidth, m_boundary.top + halfHeight, halfWidth, halfHeight);
	const Rectf sw(m_boundary.left, m_boundary.top + halfHeight, halfWidth, halfHeight);

	const Rectf pos = pEntity->getComponent<KCColliderBase>()->getBoundingBox();

	if (nw.intersects(pos))
	{
		return northWest;
	}

	if (ne.intersects(pos))
	{
		return northEast;
	}

	if (se.intersects(pos))
	{
		return southEast;
	}

	if (sw.intersects(pos))
	{
		return southWest;
	}

	return LeavesIdentifier::noLeaf;
}

std::stack<LeavesIdentifier> Krawler::KQuadtree::getLeavesEnum(KEntity * pEntity) const
{
	std::stack<LeavesIdentifier> leavesStack;
	const float halfWidth = m_boundary.width / 2.0f, halfHeight = m_boundary.height / 2.0f;
	const Rectf nw(m_boundary.left, m_boundary.top, halfWidth, halfHeight);
	const Rectf ne(m_boundary.left + halfWidth, m_boundary.top, halfWidth, halfWidth);
	const Rectf se(m_boundary.left + halfWidth, m_boundary.top + halfHeight, halfWidth, halfHeight);
	const Rectf sw(m_boundary.left, m_boundary.top + halfHeight, halfWidth, halfHeight);

	const Rectf& bounds = pEntity->getComponent<KCColliderBase>()->getBoundingBox();

	if (nw.intersects(bounds))
	{
		leavesStack.push(northWest);
	}

	if (ne.intersects(bounds))
	{
		leavesStack.push(northEast);
	}

	if (se.intersects(bounds))
	{
		leavesStack.push(southEast);
	}

	if (sw.intersects(bounds))
	{
		leavesStack.push(southWest);
	}

	return leavesStack;
}
