#include "Physics\KPhysicsBody.h"
#include "GameObject\KGameObject.h"

//public functions 
using namespace Krawler;
using namespace Krawler::Physics;

Krawler::Physics::KPhysicsBody::KPhysicsBody(float mass, bool subjectToGravity, KGameObject* shape)
	: mp_gameObject(shape), mb_actsUnderGravity(subjectToGravity)
{
	setMass(mass);
}

Krawler::Physics::KPhysicsBody::~KPhysicsBody()
{

}

void Krawler::Physics::KPhysicsBody::step(float delta, float pixelsToMetres)
{
	m_prevPosition = mp_gameObject->getPosition();

	Vec2f acceleration = (m_massData.invMass * m_force) * delta;
	
	Vec2f moveMetresPerSecond = (m_velocity + (acceleration / 2.0f)) * delta;

	mp_gameObject->move(moveMetresPerSecond / pixelsToMetres);
	//m_gameObject->move((m_velocity + (acceleration / 2.0f))* delta);
	m_velocity += acceleration;
	//const float inverseMomentofinertia = 0.01f;
	//m_physicsState.angularVelocity += m_physicsState.torque * (1.0f * inverseMomentofinertia * delta);
	//m_physicsState.orientation += m_physicsState.angularVelocity * delta;
	//mp_gameObject->setRotation(Maths::Degrees(m_physicsState.orientation));
	//m_gameObject->move(m_velocity * delta * METRES_TO_PIXELS);

	if (m_velocity.x < (1.0e-5f))
		m_velocity.x = 0.0f;
	if (m_velocity.y < (1.0e-5f))
		m_velocity.y = 0.0f;

	m_force = Vec2f(0.f, 0.f);
	m_physicsState.torque = 0.0f;
}


void Krawler::Physics::KPhysicsBody::setMass(float m)
{
	m_massData.setMass(m);
}

KRAWLER_API void Krawler::Physics::KPhysicsBody::setPosition(Vec2f pos)
{
	mp_gameObject->setPosition(pos);
}

void Krawler::Physics::KPhysicsBody::moveBody(Vec2f translation)
{
	mp_gameObject->move(translation);
}

const std::wstring Krawler::Physics::KPhysicsBody::getGameObjectName() const
{
	return mp_gameObject->getObjectName();
}
