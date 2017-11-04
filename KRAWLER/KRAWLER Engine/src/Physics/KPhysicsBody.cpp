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
	Vec2f acceleration = (m_massData.invMass * m_force) * delta;

	Vec2f moveMetresPerSecond = (m_velocity + (acceleration / 2.0f)) * delta;

	mp_gameObject->move(moveMetresPerSecond / pixelsToMetres);
	//m_gameObject->move((m_velocity + (acceleration / 2.0f))* delta);
	m_velocity += acceleration;

	//m_gameObject->move(m_velocity * delta * METRES_TO_PIXELS);

	m_force = Vec2f(0.f, 0.f);
}


void Krawler::Physics::KPhysicsBody::setMass(float m)
{
	m_massData.setMass(m);
}

void Krawler::Physics::KPhysicsBody::moveBody(Vec2f translation)
{
	mp_gameObject->move(translation);
}

//void Krawler::Physics::KPhysicsBody::addCollidedBody(KPhysicsBody * b)
//{
//	auto& list = m_gameObject->getCollidedWithList();
//	for (auto& collided : list)
//	{
//		if (collided == b)
//		{
//			return;
//		}
//	}
//	m_gameObject->addCollidedBody(b);
//}

const std::wstring Krawler::Physics::KPhysicsBody::getGameObjectName() const
{
	return mp_gameObject->getObjectName();
}
