#include "KApplication.h"
#include "Components\KCBody.h"
#include "Physics\b2dConversion.h"

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

	m_polygonShape.SetAsBox(m_halfBounds.x, m_halfBounds.y);
	b2FixtureDef fixtureDef = convertToB2FixtureDef(m_matDef);
	fixtureDef.shape = &m_polygonShape;
	auto f = m_pB2Body->CreateFixture(&fixtureDef);

	return KInitStatus::Success;
}

void KCBody::fixedTick()
{
	const Vec2f translation = b2ToVec2f(m_pB2Body->GetPosition());
	const float rotation = Maths::Degrees(m_pB2Body->GetAngle());
	getEntity()->getTransform()->setTranslation(translation);
	getEntity()->getTransform()->setRotation(rotation);
}

void KCBody::setPosition(const Vec2f& position) const
{
	m_pB2Body->SetTransform(Vec2fTob2(position), m_pB2Body->GetAngle());
}

void KCBody::setRotiation(float rotation) const
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