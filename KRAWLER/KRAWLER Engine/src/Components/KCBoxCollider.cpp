#include "Components\KCBoxCollider.h"
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

KCBoxCollider::KCBoxCollider(KEntity * pEntity, const Vec2f& size) :
	KComponentBase(pEntity), m_size(size)
{
	m_localVertices[0] = Vec2f(0.0f, 0.0f);
	m_localVertices[1] = Vec2f(size.x, 0.0f);
	m_localVertices[2] = Vec2f(size.x, size.y);
	m_localVertices[3] = Vec2f(0.0f, size.y);
}

void KCBoxCollider::collisionCallback(KEntity * pEntity)
{
	for (auto& callback : m_callbacks)
	{
		callback(pEntity);
	}
}

void KCBoxCollider::tick()
{
	KCTransform* pTransform = getEntity()->getComponent<KCTransform>();

	for (int32 i = 0; i < 4; ++i)
	{
		m_globalVertices[i] = pTransform->getTransform().transformPoint(m_localVertices[i]);
	}
	updateAABB();
}

KRAWLER_API bool Krawler::Components::KCBoxCollider::checkIntersects(KCBoxCollider * pCollider)
{
	//KCTransform* const pTransformA = getEntity()->getComponent<KCTransform>();
	//KCTransform* const pTransformB = pCollider->getEntity()->getComponent<KCTransform>();

	//sf::FloatRect boxA = pTransformA->getTransform().transformRect(sf::FloatRect(0.0f, 0.0, m_size.x, m_size.y));
	//sf::FloatRect boxB = pTransformB->getTransform().transformRect(sf::FloatRect(0.0f, 0.0, m_size.x, m_size.y));

	return m_aabb.intersects(pCollider->getBounds());
}

void Krawler::Components::KCBoxCollider::updateAABB()
{
	KCTransform* const pTransformA = getEntity()->getComponent<KCTransform>();
	m_aabb = pTransformA->getTransform().transformRect(sf::FloatRect(0.0f, 0.0, m_size.x, m_size.y));
}
