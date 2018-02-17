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

const Rectf & Krawler::Components::KCCircleCollider::getBoundingBox()
{
	updateCentrePosition();
	constexpr float mutliplier = 2.0f;
	m_boundingBox.left = m_centrePos.x - m_radius * mutliplier;
	m_boundingBox.top = m_centrePos.y - m_radius * mutliplier;
	m_boundingBox.height = m_boundingBox.width = 2 * (m_radius*mutliplier);
	return m_boundingBox;
}

void Krawler::Components::KCCircleCollider::updateCentrePosition()
{
	KCTransform* const pTransform = getEntity()->getComponent<KCTransform>();
	Vec2f topLeft;

	topLeft = pTransform->getPosition();// -pTransform->getOrigin();

	if (pTransform->getOrigin() == Vec2f(m_radius, m_radius))
	{
		m_centrePos = pTransform->getTransform().transformPoint(m_radius,m_radius);
		return;
	}

	m_centrePos = topLeft + Vec2f(m_radius, m_radius);
}
