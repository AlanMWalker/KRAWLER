#include "Physics\KPhysicsScene.h"

//public functions 

using namespace Krawler;
using namespace Krawler::Physics;
using namespace Krawler::Maths;


KPhysicsScene::KPhysicsScene()
{
	m_bodies.clear();
}

void KPhysicsScene::step(float delta)
{
	for (auto & b : m_bodies)
	{
		if (b->isSubjectToGravity())
		{
			b->applyForce(m_gravity * b->getMass() *b->getMaterialData().gravityScale);
		}
	}

	for (auto& b : m_bodies)
	{
		if (!b->isBodyInUse())
		{
			continue;
		}
		//std::async(std::launch::async, &KPhysicsBody::step, b, delta, m_pixelsToMetres);
		b->step(delta, m_pixelsToMetres);
	}

	generateCollisionPairs(); //Broadphase pairs 
	handleCollisionPairs();

	for (auto& b : m_bodies)
	{
		if (!b->isStaticBody())
		{
			continue;
		}
		b->resetForce();
		b->resetVelocity();
	}
}

void KPhysicsScene::setPixelsToMetresScale(float scale)
{
	m_pixelsToMetres = scale;
}

KPhysicsBody* KPhysicsScene::addBody(KGameObject* object, float mass, bool subjectToGravity)
{
	KPhysicsBody* body{ nullptr };

	m_bodies.push_back(new KPhysicsBody(mass, subjectToGravity, object));

	KCHECK(m_bodies.back());

	return(m_bodies.back());
}

void KPhysicsScene::removeFromScene(KPhysicsBody * pBody)
{
	auto& i = m_bodies.begin(); // create an iterator

	for (i; i != m_bodies.end(); ++i)
	{
		if (*i == pBody)
		{ //if the iterator is on the correct body we want to break out of the loop
			break;
		}
	}

	m_bodies.erase(i);
}

void KPhysicsScene::cleanupScene()
{
	for (KPhysicsBody* b : m_bodies)
	{
		KFREE(b);
	}

	m_bodies.clear();
}

bool KPhysicsScene::doesAABBIntersect(const sf::FloatRect & aabb)
{
	for (auto& b : m_bodies)
	{
		if (aabb.intersects(b->getGameObject().getAlignedGlobalBounds()))
		{
			return true;
		}
	}
	return false;
}

void Krawler::Physics::KPhysicsScene::LerpPositions(float alpha)
{
	static bool bIsFirstAlpha = true;

	if (bIsFirstAlpha)
	{
		bIsFirstAlpha = false;
		return;
	}
	for (auto& body : m_bodies)
	{
		const Vec2f currentPos = body->getGameObject().getPosition();
		const Vec2f lastPos = body->getPreviousPosition();
		//TODO handle oriented bodies in this process once they're integrated into physics

		//currentState * alpha +  previousState * (1.0 - alpha);
		const Vec2f newPos = (currentPos * alpha) + (lastPos * (1.0f - alpha));
		body->setPosition(newPos);
	}
}

//private functions

void KPhysicsScene::resolveCollision(const KCollisionData& collData)
{
	KPhysicsBody* bodyA = collData.bodyA;
	KPhysicsBody* bodyB = collData.bodyB;

	float e = Max(bodyB->getMaterialData().restitution, bodyA->getMaterialData().restitution);
	Vec2f relativeVelocity = bodyB->getVelocity() - bodyA->getVelocity();
	float velocityAlongNormal = DotProduct(relativeVelocity, collData.collisionNormal);

	float j = -(1 + e) * velocityAlongNormal;

	j /= (bodyA->getInverseMass() + bodyB->getInverseMass());

	Vec2f impulse = j * collData.collisionNormal;

	bodyA->setVelocity(bodyA->getVelocity() - (impulse * bodyA->getInverseMass()));
	bodyB->setVelocity(bodyB->getVelocity() + (impulse * bodyB->getInverseMass()));

	/* Friction Application */
	relativeVelocity = { bodyB->getVelocity() - bodyA->getVelocity() };

	//get the tangent vector
	Vec2f tangent = relativeVelocity - (DotProduct(relativeVelocity, collData.collisionNormal) * collData.collisionNormal);
	tangent = Normalise(tangent);

	float jt = -DotProduct(relativeVelocity, tangent);
	jt = jt / (bodyA->getInverseMass() + bodyB->getInverseMass());

	const float mu = GetLength(Vec2f(bodyA->getMaterialData().staticFriction, bodyB->getMaterialData().staticFriction)); //static coefficient

	const float dMu = GetLength(Vec2f(bodyA->getMaterialData().dynamicFriction, bodyB->getMaterialData().dynamicFriction)); //dynamic coefficient

	Vec2f frictionImpulse;
	if (abs(jt) < j * mu)
	{
		frictionImpulse = jt * tangent;
	}
	else
	{
		frictionImpulse = -j * tangent * dMu;
	}


	if (!bodyA->isStaticBody())
	{
		bodyA->applyImpulse(-frictionImpulse, tangent);
		//bodyA->setVelocity(bodyA->getVelocity() - (frictionImpulse * bodyA->getInverseMass()));
	}
	if (!bodyB->isStaticBody())
	{
		bodyB->applyImpulse(frictionImpulse, tangent);

		//bodyB->setVelocity(bodyB->getVelocity() + (frictionImpulse * bodyB->getInverseMass()));
	}
}


void KPhysicsScene::correctPosition(const KCollisionData& collData)
{
	KPhysicsBody* bodyA = collData.bodyA;
	KPhysicsBody* bodyB = collData.bodyB;

	Vec2f correction = Max(collData.penetration - m_slop, 0.0f) / (bodyA->getInverseMass() + bodyB->getInverseMass()) * m_correctionPercentage * collData.collisionNormal;
	bodyA->moveBody(-bodyA->getInverseMass() * correction);
	bodyB->moveBody(bodyB->getInverseMass() * correction);
}

void KPhysicsScene::impulseResolution(KPhysicsBody & bodyA, KPhysicsBody & bodyB, Vec2f collisionNormal, float dp)
{
	float e = Max(bodyB.getMaterialData().restitution, bodyA.getMaterialData().restitution);
	float j = -(1 + e) * dp;
	j /= (bodyA.getInverseMass() + bodyB.getInverseMass());

	Vec2f impulse = j * collisionNormal;

	bodyA.setVelocity(bodyA.getVelocity() - (impulse * bodyA.getInverseMass()));
	bodyB.setVelocity(bodyB.getVelocity() + (impulse * bodyB.getInverseMass()));

	/* Friction Application */
	Vec2f relativeVelocity = bodyB.getVelocity() - bodyA.getVelocity();

	//get the tangent vector
	Vec2f tangent = relativeVelocity - (DotProduct(relativeVelocity, collisionNormal) * collisionNormal);
	tangent = Normalise(tangent);

	float jt = -DotProduct(relativeVelocity, tangent);
	jt = jt / (bodyA.getInverseMass() + bodyB.getInverseMass());

	float mu{ 0.3f }; //static coefficient
	float dMu{ 0.25f }; //dynamic coefficient

	Vec2f frictionImpulse;
	if (abs(jt) < j * mu)
	{
		frictionImpulse = jt * tangent;
	}
	else
	{
		frictionImpulse = -j * tangent * dMu;
	}

	bodyA.setVelocity(bodyA.getVelocity() - (frictionImpulse * bodyA.getInverseMass()));
	bodyB.setVelocity(bodyB.getVelocity() + (frictionImpulse * bodyB.getInverseMass()));
}

void KPhysicsScene::generateCollisionPairs()
{
	m_uniquePairs.clear();

	for (auto i = m_bodies.begin(); i != m_bodies.end(); ++i)
	{
		for (auto j = i + 1; j != m_bodies.end(); ++j)
		{
			KPhysicsBody* bodyA = *i;
			KPhysicsBody* bodyB = *j;

			if (!bodyA->isBodyInUse() || !bodyB->isBodyInUse())
			{
				continue;
			}

			std::pair<KPhysicsBody*, KPhysicsBody*> pair(nullptr, nullptr);

			if (bodyA == bodyB)
			{ //If same body ignore
				continue;
			}
			const KGameObject& objA(bodyA->getGameObject());
			const KGameObject& objB(bodyB->getGameObject());

			if (!AABBvsAABB(objA.getFixedGlobalBounds(), objB.getFixedGlobalBounds()))
			{// if no collision, continue to next pair
				continue;
			}

			pair.first = bodyA;
			pair.second = bodyB;

			m_uniquePairs.push_back(pair);
		}
	}
}

void KPhysicsScene::handleCollisionPairs()
{
	for (auto& pair : m_uniquePairs)
	{
		KCollisionData data;
		data.bodyA = pair.first;
		data.bodyB = pair.second;

		if (m_collisionFunctions[data.bodyA->getShapeType()][data.bodyB->getShapeType()](data))
		{
			resolveCollision(data);
			correctPosition(data);
		}
	}
}

KRAWLER_API extern bool Krawler::Physics::AABBvsAABB(const sf::FloatRect & rectA, const sf::FloatRect & rectB)
{
	return(rectA.left + rectA.width > rectB.left && rectA.left < rectB.left + rectB.width) &&
		(rectA.top + rectA.height > rectB.top && rectA.top < rectB.top + rectB.height);
}
