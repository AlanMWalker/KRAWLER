#include "KApplication.h"
#include "Components\KCBody.h"
#include "Physics\b2dConversion.h"

#include "Components/KCCircleCollider.h"
#include "Components/KCBoxCollider.h"
#include "box2d/box2d.h"
using namespace Krawler;
using namespace Krawler::Components;

// Conversion Functions for data structures
b2BodyDef convertToB2BodyDef(const KBodyDef& def)
{
	b2BodyDef bodyDef;
	bodyDef.type = KCAST(b2BodyType, def.bodyType);
	bodyDef.bullet = def.bBullet;
	bodyDef.gravityScale = def.gravityScale;
	bodyDef.awake = def.bAwake;
	bodyDef.allowSleep = def.bAllowSleep;
	bodyDef.angle = def.angle;
	bodyDef.position = Vec2fTob2(def.position);
	bodyDef.linearVelocity = Vec2fTob2(def.linearVelocity);
	bodyDef.angularVelocity = def.angularVelocity;
	bodyDef.active = def.bActive;
	bodyDef.fixedRotation = def.bFixedRotation;
	bodyDef.linearDamping = def.linearDamping;
	bodyDef.angularDamping = def.angularDamping;

	return bodyDef;
}

b2FixtureDef convertToB2FixtureDef(const KMatDef& def)
{
	b2FixtureDef fixtureDef;
	fixtureDef.density = def.density;
	fixtureDef.friction = def.friction;
	fixtureDef.restitution = def.restitution;

	return fixtureDef;
}

// Class Functions

KCBody::KCBody(KEntity& entity, const Vec2f& bounds)
	: KComponentBase(&entity), m_bounds(bounds), m_halfBounds(m_bounds * 0.5f)
{
}

KCBody::KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef)
	: KComponentBase(&entity), m_bounds(bounds), m_halfBounds(m_bounds * 0.5f), m_bodyDef(bodyDef)
{
}

KCBody::KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef, const KMatDef& matDef)
	: KComponentBase(&entity), m_bounds(bounds), m_halfBounds(m_bounds * 0.5f), m_bodyDef(bodyDef), m_matDef(matDef)
{

}

KInitStatus KCBody::init()
{
	auto pApp = GET_APP();
	auto& physWorld = pApp->getPhysicsWorld();

	auto convertedBodyDef = convertToB2BodyDef(m_bodyDef);
	m_pB2Body = physWorld.addNewBody(convertedBodyDef);

	const float ppm = physWorld.getPPM();

	m_polygonShape.SetAsBox(m_halfBounds.x / ppm, m_halfBounds.y / ppm);
	b2FixtureDef fixtureDef = convertToB2FixtureDef(m_matDef);
	fixtureDef.shape = &m_polygonShape;
	m_pCurrentFixture = m_pB2Body->CreateFixture(&fixtureDef);

	return KInitStatus::Success;
}

void KCBody::fixedTick()
{
	const float ppm = GET_APP()->getPhysicsWorld().getPPM();
	const Vec2f translation = b2ToVec2f(m_pB2Body->GetPosition());
	const float rotation = Maths::Degrees(m_pB2Body->GetAngle());
	getEntity()->m_pTransform->setPosition(translation * ppm);
	getEntity()->m_pTransform->setRotation(rotation);
}

void KCBody::onEnterScene()
{
	auto pCollider = getEntity()->getComponent<KCColliderBase>();
	if (pCollider != nullptr)
	{
		m_pB2Body->DestroyFixture(m_pCurrentFixture);
		b2FixtureDef fixtureDef = convertToB2FixtureDef(m_matDef);
		fixtureDef.shape = pCollider->getB2Shape().lock().get();
		m_pCurrentFixture = m_pB2Body->CreateFixture(&fixtureDef);
	}
}

void KCBody::onExitScene()
{
	auto& physWorld = GET_APP()->getPhysicsWorld();
	physWorld.removeBody(m_pB2Body);
	m_pB2Body = nullptr;
}

void KCBody::setPosition(const Vec2f& position) const
{
	const float ppm = GET_APP()->getPhysicsWorld().getPPM();

	m_pB2Body->SetTransform(Vec2fTob2(position / ppm), m_pB2Body->GetAngle());
}

void KCBody::setRotation(float rotation) const
{
	// Box2D requires angle in radians, but internally
	// we use degrees, so convert.
	const float rad = Maths::Radians(rotation);
	m_pB2Body->SetTransform(m_pB2Body->GetPosition(), rad);
}

void KCBody::setActivity(bool bIsActive) const
{
	m_pB2Body->SetActive(bIsActive);
}

Vec2f KCBody::getLinearVelocity() const
{
	return Vec2f(b2ToVec2f(m_pB2Body->GetLinearVelocity()));
}

void KCBody::setLinearVelocity(const Vec2f& velocity)
{
	m_pB2Body->SetLinearVelocity(Vec2fTob2(velocity));
}

float KCBody::getAngularVelocity() const
{
	return m_pB2Body->GetAngularVelocity();
}

void KCBody::setAngularVelocity(float velocity)
{
	m_pB2Body->SetAngularVelocity(velocity);
}

void KCBody::applyForce(const Vec2f& force, const Vec2f& point, bool wake)
{
	m_pB2Body->ApplyForce(Vec2fTob2(force), Vec2fTob2(force), wake);
}

void KCBody::applyForceToCentre(const Vec2f& force, bool wake)
{
	m_pB2Body->ApplyForceToCenter(Vec2fTob2(force), wake);
}

void KCBody::applyTorque(float torque, bool wake)
{
	m_pB2Body->ApplyTorque(torque, wake);
}

void KCBody::applyLinearImpulse(const Vec2f& impulse, const Vec2f& point, bool wake)
{
	m_pB2Body->ApplyLinearImpulse(Vec2fTob2(impulse), Vec2fTob2(point), wake);
}

void KCBody::applyLinearImpulseToCenter(const Vec2f& impulse, bool wake)
{
	m_pB2Body->ApplyLinearImpulseToCenter(Vec2fTob2(impulse), wake);
}

void KCBody::getMass() const
{
	m_pB2Body->GetMass();
}

float KCBody::getGravityScale() const
{
	return m_pB2Body->GetGravityScale();
}

void KCBody::setGravityScale(float scale)
{
	m_pB2Body->SetGravityScale(scale);
}

void KCBody::setDensity(float density)
{
	m_pB2Body->GetFixtureList()->SetDensity(density);
}
