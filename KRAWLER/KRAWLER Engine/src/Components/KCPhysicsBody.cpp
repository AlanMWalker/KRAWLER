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

	/*const float dt = KApplication::getApp()->getPhysicsDelta();
	auto pPhysWorld = KApplication::getApp()->getPhysicsWorld();
	KCTransform* const pTransform = getEntity()->getComponent<KCTransform>();

	m_prevPosition = pTransform->getPosition();
	const Vec2f acceleration = pPhysWorld->getPhysicsWorldProperties().gravity + (m_properties.invMass * m_force);
	m_velocity += acceleration * dt;

	Vec2f moveVec = m_velocity * dt;
	moveVec *= (1.0f / pPhysWorld->getPhysicsWorldProperties().metresToPixels);

	pTransform->move(moveVec);*/

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
