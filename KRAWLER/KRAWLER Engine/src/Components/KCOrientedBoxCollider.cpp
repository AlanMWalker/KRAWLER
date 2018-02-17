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

	for (int i = 0; i < FaceCount; ++i)
	{
		Vec2f v1, v2;
		v1 = m_vertices[i];
		v2 = m_vertices[i + 1 > FaceCount ? 0 : i + 1];
		m_normals[i] = Normalise(v2 - v1);
		m_normals[i] = Vec2f(m_normals[i].y, -m_normals[i].x);
	}
	m_orientation.setOrientation(0);
}

const Rectf & KCOrientedBoxCollider::getBoundingBox()
{
	Vec2f pos = m_pTransform->getTranslation();
	m_boundingBox = Rectf(m_pTransform->getTransform().transformPoint(pos), m_size);
	return m_boundingBox;
}

void KCOrientedBoxCollider::tick()
{
	const Vec2f& origin = m_pTransform->getOrigin();
	if (origin != Vec2f(0, 0))
	{
		m_vertices[0] = Vec2f(-origin.x, -origin.y);
		m_vertices[1] = Vec2f(-origin.x + m_size.x, -origin.y);
		m_vertices[2] = Vec2f(-origin.x + m_size.x, -origin.y + m_size.y);
		m_vertices[3] = Vec2f(-origin.x, -origin.y + m_size.y);
	}
	m_orientation.setOrientation(m_pTransform->getRotation());
	//Vec2f pA = m_pTransform->getTransform().transformPoint(0, 0);
	//Vec2f pB = m_orientation* Vec2f(0, 0) + m_pTransform->getTranslation();
}

void KCOrientedBoxCollider::fixedTick()
{
	m_orientation.setOrientation(m_pTransform->getRotation());
}

Vec2f KCOrientedBoxCollider::getOrientedFaceNormal(int faceIdx)const
{
	KCHECK(faceIdx >= 0 && faceIdx < FaceCount);

	return m_orientation * m_normals[faceIdx];
}

const Vec2f& KCOrientedBoxCollider::getNormal(int faceIdx) const
{
	KCHECK(faceIdx >= 0 && faceIdx < FaceCount);
	return m_normals[faceIdx];
}

Vec2f KCOrientedBoxCollider::getVertexWorldPosition(int vertIdx) const
{
	KCHECK(vertIdx >= 0 && vertIdx < FaceCount);
	const Vec2f& translation = m_pTransform->getTranslation();
	return (m_orientation * m_vertices[vertIdx]) + translation;
}

const Vec2f & KCOrientedBoxCollider::getVertexLocalPosition(int vertIdx) const
{
	KCHECK(vertIdx >= 0 && vertIdx < FaceCount);
	return m_vertices[vertIdx];
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
