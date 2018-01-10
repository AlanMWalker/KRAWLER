#include "Components\KCCircleCollider.h"
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

KCCircleCollider::KCCircleCollider(KEntity* pEntity, float radius)
	: KCColliderBase(pEntity, KCColliderType::Circle), m_radius(radius)

{

}

const Vec2f & KCCircleCollider::getCentrePosition()
{
	updateCentrePosition();
	return m_centrePos;
}

float Krawler::Components::KCCircleCollider::getRadius() const
{
	return m_radius;
}

void Krawler::Components::KCCircleCollider::updateCentrePosition()
{
	KCTransform* const pTransform = getEntity()->getComponent<KCTransform>();

	Vec2f topLeft;// top left co-ord of the entity

	topLeft = pTransform->getOrigin();

	topLeft = -(Vec2f(topLeft.x * pTransform->getScale().x, topLeft.y * pTransform->getScale().y));

	topLeft = pTransform->getTransform().transformPoint(0, 0);

	m_centrePos = topLeft + Vec2f(m_radius, m_radius);
}
