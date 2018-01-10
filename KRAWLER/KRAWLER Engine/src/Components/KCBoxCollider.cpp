#include "Components\KCBoxCollider.h"
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

KCBoxCollider::KCBoxCollider(KEntity * pEntity, const Vec2f& size) :
	KCColliderBase(pEntity, KCColliderType::AABB), m_size(size), m_halfSize(size*0.5f)
{
	m_pTransform = getEntity()->getComponent<KCTransform>();
}

const Rectf & KCBoxCollider::getBounds()
{
	updateAABB();
	return m_aabb;
}

Vec2f KCBoxCollider::getTopLeftCoord() const
{
	Vec2f coord;
	coord = m_pTransform->getOrigin();
	coord = -(Vec2f(coord.x * m_pTransform->getScale().x, coord.y * m_pTransform->getScale().y));
	return m_pTransform->getTransform().transformPoint(0, 0);
}

void KCBoxCollider::updateAABB()
{
	m_aabb = m_pTransform->getTransform().transformRect(sf::FloatRect(0.0f, 0.0, m_size.x, m_size.y));
}
