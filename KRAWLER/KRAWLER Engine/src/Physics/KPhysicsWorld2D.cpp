#include "Physics/KPhysicsWorld2D.h"
#include "Physics/b2dConversion.h"

using namespace Krawler;
using namespace Krawler::Physics;


KPhysicsWorld2D::KPhysicsWorld2D()
{

}

KPhysicsWorld2D::~KPhysicsWorld2D()
{
}

KInitStatus KPhysicsWorld2D::initialiseWorld()
{
	if (!m_pBox2DWorld)
	{
		m_pBox2DWorld = new b2World(Vec2fTob2(m_gravity));
	}

	return KInitStatus::Success;
}

void KPhysicsWorld2D::cleanupWorld()
{
	KFREE(m_pBox2DWorld);
}

void KPhysicsWorld2D::stepWorld(float physicsStep)
{
	m_pBox2DWorld->Step(physicsStep, m_velocityIterations, m_positionIterations);
}

void KPhysicsWorld2D::setGravity(const Krawler::Vec2f& g)
{
	m_gravity = g;
	m_pBox2DWorld->SetGravity(Vec2fTob2(g));
}

Vec2f KPhysicsWorld2D::getGravity() const
{
	return m_gravity;
}

b2Body* KPhysicsWorld2D::addNewBody(const b2BodyDef& def)
{
	return  m_pBox2DWorld->CreateBody(&def);
}

void KPhysicsWorld2D::removeBody(b2Body* const pBody)
{
	m_pBox2DWorld->DestroyBody(pBody);
}

void KPhysicsWorld2D::rayCast(const Vec2f& start, const Vec2f& end)
{
	RaycastCB cb;
	if (GetSquareLength(end - start) <= 0.0f)
		return;

	m_pBox2DWorld->RayCast(&cb, Vec2fTob2(start), Vec2fTob2(end));
}
