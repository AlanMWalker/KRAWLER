#include <Components\KCOrientedBoxCollider.h>
#include <Components\KCTransform.h>

using namespace Krawler;
using namespace Krawler::Components;

KCOrientedBoxCollider::KCOrientedBoxCollider(KEntity* pEntity, const Vec2f& size)
	: KCColliderBase(pEntity, OBB), m_size(size)
{
	m_pTransform = getEntity()->getComponent<KCTransform>();
	m_vertices[0] = Vec2f(0, 0);
	m_vertices[1] = Vec2f(m_size.x, 0);
	m_vertices[2] = m_size;
	m_vertices[3] = Vec2f(0, m_size.y);
	m_orientation.setOrientation(0);
}

const Rectf & KCOrientedBoxCollider::getBoundingBox()
{
	Vec2f pos = getEntity()->getComponent<KCTransform>()->getTransform().transformPoint(Vec2f(0, 0));
	m_boundingBox = Rectf(pos, m_size);
	return m_boundingBox;
}

void KCOrientedBoxCollider::tick()
{
	for (auto& vert : m_vertices)
	{
		vert = m_pTransform->getTransform().transformPoint(vert);
	}
	m_orientation.setOrientation(m_pTransform->getRotation());
}

Vec2f KCOrientedBoxCollider::getFaceNormal(int faceIdx)const
{
	return m_orientation * m_normals[faceIdx];
}

Vec2f KCOrientedBoxCollider::getVertexPosition(int vertIdx) const
{
	const Vec2f& translation = m_pTransform->getTranslation();

	return m_orientation * (m_vertices[vertIdx] + translation);
}

Vec2f KCOrientedBoxCollider::getSupport(const Vec2f& dir) const
{
	float bestProjection = -FLT_MAX;
	Vec2f bestVertex;

	for (auto& vert : m_vertices)
	{
		float projection = DotProduct(vert, dir);

		if (projection > bestProjection)
		{
			bestVertex = vert;
			bestProjection = projection;
		}
	}

	return bestVertex;
}
