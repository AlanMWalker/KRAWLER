#include <Components\KCCircleCollider.h>
#include <Components\KCTransform.h>
#include "KApplication.h"

#include "box2d\b2_circle_shape.h"


using namespace Krawler;
using namespace Krawler::Components;

KCCircleCollider::KCCircleCollider(KEntity* pEntity, float radius)
	: KCColliderBase(pEntity, KCColliderType::Circle), m_radius(radius)

{
	m_pTransform = pEntity->getComponent<KCTransform>();
	auto p = getB2Shape().lock();
	m_pCircleShape = std::dynamic_pointer_cast<b2CircleShape>(p);
	auto ppm = GET_APP()->getPhysicsWorld().getPPM();
	m_pCircleShape.lock()->m_radius = m_radius / ppm;
}

const Vec2f& KCCircleCollider::getCentrePosition()
{
	updateCentrePosition();
	return m_centrePos;
}

float Krawler::Components::KCCircleCollider::getRadius() const
{
	return m_radius;
}

const Rectf& Krawler::Components::KCCircleCollider::getBoundingBox()
{
	updateCentrePosition();
	constexpr float mutliplier = 2.0f;
	m_boundingBox.left = m_centrePos.x - m_radius * mutliplier;
	m_boundingBox.top = m_centrePos.y - m_radius * mutliplier;
	m_boundingBox.height = m_boundingBox.width = 2 * (m_radius * mutliplier);
	return m_boundingBox;
}

void Krawler::Components::KCCircleCollider::updateCentrePosition()
{
	Vec2f topLeft;

	topLeft = m_pTransform->getPosition();

	if (m_pTransform->getOrigin() == Vec2f(m_radius, m_radius))
	{
		m_centrePos = m_pTransform->getTransform().transformPoint(m_radius, m_radius);
		return;
	}

	m_centrePos = topLeft + Vec2f(m_radius, m_radius);
}
