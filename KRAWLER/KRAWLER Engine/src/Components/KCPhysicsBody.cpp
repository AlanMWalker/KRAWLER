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

KPhysicsBodyProperties::KPhysicsBodyProperties(float mass, float sFriction, float dFriction, float restit)
	: staticFriction(sFriction), dynamicFriction(dFriction), restitution(restit)
{
	setMass(mass);
}

//KCPhysicsBody

KCPhysicsBody::KCPhysicsBody(KEntity* pEntity, KPhysicsBodyProperties properties)
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
	if (m_properties.mass == 0.0f || m_bIsStatic)
	{
		return;
	}
	m_force = Vec2f(0.0f, 0.0f);
}

void Krawler::Components::KCPhysicsBody::applyForce(const Vec2f & force)
{
	m_force += force;
}

void KPhysicsBodyProperties::setMass(float inMass)
{
	if (inMass != 0.0f)
	{
		mass = inMass;
		invMass = 1.0f / inMass;
	}
	else
	{
		mass = 0.0f;
		invMass = 0.0f;
	}
}

void KPhysicsBodyProperties::computeMass(float density, KCColliderType colliderType)
{
	density = 0.0;
	colliderType = KCColliderType::Circle;
}
