#include "Components\KCPhysicsBody.h"	
#include "KApplication.h"

using namespace Krawler;
using namespace Krawler::Components;

KPhysicsBodyProperties::KPhysicsBodyProperties()

{
	setMass(1.0f);
	restitution = 1.0f;
	staticFriction = 0.0;
	dynamicFriction = 0.0;
}

//KCPhysicsBody

KCPhysicsBody::KCPhysicsBody(KEntity * pEntity, KPhysicsBodyProperties properties)
	:KComponentBase(pEntity), m_properties(properties)
{
}

KInitStatus KCPhysicsBody::init()
{
	m_prevPosition = getEntity()->getComponent<KCTransform>()->getPosition();

	return KInitStatus::Success;
}

void KCPhysicsBody::fixedTick()
{
	const float dt = KApplication::getApp()->getPhysicsDelta();
	auto pPhysWorld = KApplication::getApp()->getPhysicsWorld();
	KCTransform* const pTransform = getEntity()->getComponent<KCTransform>();

	m_prevPosition = pTransform->getPosition();

	applyForce(pPhysWorld->getPhysicsWorldProperties().gravity * m_properties.mass); //apply gravity

	const Vec2f acceleration = (m_properties.invMass * m_force) * dt;

	const Vec2f moveMetresPerSecond = (m_velocity + (acceleration / 2.0f)) * dt;

	pTransform->move(moveMetresPerSecond / pPhysWorld->getPhysicsWorldProperties().pixelsToMetres);

	m_velocity += acceleration;

	m_force = Vec2f(0.0f, 0.0f);
}

void Krawler::Components::KCPhysicsBody::applyForce(const Vec2f & force)
{
	m_force += force;
}

void KPhysicsBodyProperties::setMass(float inMass)
{
	if (mass != 0.0f)
	{
		mass = inMass;
		invMass = 1.0f / mass;
	}
	else
	{
		mass = 0.0f;
		invMass = 1.0f;
	}
}

void KPhysicsBodyProperties::computeMass(float density, KColliderType colliderType)
{
	//switch (colliderType)
	//{
	//default:
	//case AABB:
	//	float mass = ;
	//	setMass();
	//	break;
	//
	//case Circle:
	//	// cast to circle collider
	//	break;
	//}
}
