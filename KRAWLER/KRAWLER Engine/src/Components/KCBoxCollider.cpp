#include "Components\KCBoxCollider.h"
#include "Components\KCTransform.h"

#include "box2d\b2_polygon_shape.h"

using namespace Krawler;
using namespace Krawler::Components;

KCBoxCollider::KCBoxCollider(KEntity * pEntity, const Vec2f& size) :
	KCColliderBase(pEntity, KCColliderType::AABB), m_size(size), m_halfSize(size*0.5f)
{
	m_pTransform = getEntity()->getComponent<KCTransform>();
	auto p = getB2Shape().lock();
	m_pBoxShapeb2 = std::dynamic_pointer_cast<b2PolygonShape>(p);
}

const Rectf & KCBoxCollider::getBounds()
{
	updateAABB();
	return m_aabb;
}

const Rectf& KCBoxCollider::getBoundingBox()
{
	updateAABB();
	return m_aabb;
}

Vec2f KCBoxCollider::getTopLeftCoord() const
{
	const Vec2f& origin = m_pTransform->getOrigin();
	//if (origin == Vec2f(0, 0))
	//{
	//	//return m_pTransform->getTranslation();
	//	return m_pTransform->getPosition();
	//
	//}
	//const Vec2f coord = -(Vec2f(origin.x * m_pTransform->getScale().x, origin.y * m_pTransform->getScale().y));
	//return m_pTransform->getTranslation() - origin;

	const Vec2f pos = m_pTransform->getTransform().transformPoint(0.0f, 0.0f);
	return pos;
}

void KCBoxCollider::updateAABB()
{
	const Vec2f topLeft = getTopLeftCoord();
	const Vec2f p = m_pTransform->getTransform().transformPoint(m_pTransform->getOrigin());

	m_aabb = m_pTransform->getTransform().transformRect(Rectf(Vec2f(m_pTransform->getOrigin()), m_size));
}
