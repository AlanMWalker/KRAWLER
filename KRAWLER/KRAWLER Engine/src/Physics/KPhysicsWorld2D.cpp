#include "Physics/KPhysicsWorld2D.h"
#include "box2d/box2d.h"

using namespace Krawler;
using namespace Krawler::Physics;

// Utility Conversion Functions
static Vec2f&& b2ToVec2f(const b2Vec2& in)
{
	return Vec2f(in.x, in.y);
}

static b2Vec2&& Vec2fTob2(const Vec2f& in)
{
	return b2Vec2(in.x, in.y);
}

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
