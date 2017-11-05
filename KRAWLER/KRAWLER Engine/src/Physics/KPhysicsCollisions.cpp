#include "Physics\KPhysicsCollisions.h"
#include "Physics\KPhysicsBody.h"
#include "GameObject\KGameObject.h"
#include "Maths\KMaths.hpp"

using namespace Krawler;
using namespace Krawler::Maths;
using namespace Krawler::Physics;

bool Krawler::Physics::CirclevsCircleCollision(KCollisionData & collData)
{
	//Setup pointers to physics bodies
	KPhysicsBody* const bodyA = collData.bodyA;
	KPhysicsBody* const bodyB = collData.bodyB;

	const KGameObject& goA = bodyA->getGameObject();
	const KGameObject& goB = bodyB->getGameObject();

	const float aRadius = bodyA->getGameObject().getFixedGlobalBounds().width / 2.0f;
	const float bRadius = bodyB->getGameObject().getFixedGlobalBounds().width / 2.0f;

	const Vec2f aCentre = goA.getCentrePosition();//goA.getPosition() + Vector2f(aRadius, aRadius);
	const Vec2f bCentre = goB.getCentrePosition();//goB.getPosition() + Vector2f(bRadius, bRadius);

	Vec2f n = bCentre - aCentre; // Direction from b to a 

	float sumofRadii = aRadius + bRadius;  // calculate the sum of radius a and radius b


	if (GetSquareLength(n) > (sumofRadii*sumofRadii)) //if distance between circles > sum of radii, no collision
	{
		collData.contactCount = 0;
		return false;
	}

	float distance = GetLength(n);

	if (distance != 0)
	{
		//penetration distance is the difference between radial sum & actual distance
		collData.penetration = sumofRadii - distance;
		collData.collisionNormal = (bCentre - aCentre) / distance;
		collData.contacts[0] = collData.collisionNormal * aRadius + aCentre;
	}
	else
	{
		collData.penetration = aRadius;
		collData.collisionNormal = Vec2f(1.0f, 0.0f);
		collData.contacts[0] = aCentre;
	}

	return true;
}

bool Krawler::Physics::AABBvsAABBCollision(KCollisionData & collData)
{
	//Setup pointers to physics bodies
	KPhysicsBody* const bodyA = collData.bodyA;
	KPhysicsBody* const bodyB = collData.bodyB;

	const KGameObject& goA = bodyA->getGameObject();
	const KGameObject& goB = bodyB->getGameObject();

	Vec2f n;
	n = (goB.getPosition() + Vec2f(goB.getFixedGlobalBounds().width / 2.0f, goB.getFixedGlobalBounds().height / 2.0f))
		- (goA.getPosition() + Vec2f(goA.getFixedGlobalBounds().width / 2.0f, goA.getFixedGlobalBounds().height / 2.0f));

	float aExtent = goA.getFixedGlobalBounds().width / 2.0f;
	float bExtent = goB.getFixedGlobalBounds().width / 2.0f;

	float xOverlap = aExtent + bExtent - fabs(n.x);

	if (xOverlap > 0.0f)
	{
		float aExtent = goA.getFixedGlobalBounds().height / 2.0f;
		float bExtent = goB.getFixedGlobalBounds().height / 2.0f;

		float yOverlap = aExtent + bExtent - fabs(n.y);

		if (yOverlap > 0.0f)
		{
			if (xOverlap < yOverlap)
			{
				if (n.x < 0)
				{
					collData.collisionNormal = Vec2f(-1.0f, 0.0f);
				}
				else
				{
					collData.collisionNormal = Vec2f(1.0f, 0.0f);
				}
				collData.penetration = xOverlap;

			}
			else
			{
				if (n.y < 0)
				{
					collData.collisionNormal = Vec2f(0, -1.0f);
				}
				else
				{
					collData.collisionNormal = Vec2f(0, 1.0f);
				}
				collData.penetration = yOverlap;
			}
			return true;
		}
	}

	return false;
}

bool Krawler::Physics::AABBvsCircleCollision(KCollisionData & collData)
{
	//Setup pointers to physics bodies
	KPhysicsBody* const bodyA = collData.bodyA;
	KPhysicsBody* const bodyB = collData.bodyB;

	const KGameObject& goA = bodyA->getGameObject();
	const KGameObject& goB = bodyB->getGameObject();

	const float xExtent = goA.getFixedGlobalBounds().width / 2.0f;
	const float yExtent = goA.getFixedGlobalBounds().height / 2.0f;

	const float bRadius = goB.getFixedGlobalBounds().width / 2.0f;

	Vec2f n = (goB.getCentrePosition()) - (goA.getPosition() + Vec2f(xExtent, yExtent));
	Vec2f closest = n;

	closest.x = Clamp(-xExtent, xExtent, closest.x);
	closest.y = Clamp(-yExtent, yExtent, closest.y);

	bool inside = false;

	if (n == closest)
	{
		inside = true;
		if (fabs(n.x) > fabs(n.y))
		{
			if (closest.x > 0)
			{
				closest.x = xExtent;
			}
			else
			{
				closest.x = -xExtent;
			}
		}
		else
		{
			if (closest.y > 0)
			{
				closest.y = yExtent;
			}
			else
			{
				closest.y = -yExtent;
			}
		}
	}

	Vec2f normal = n - closest;
	float distance = GetSquareLength(normal);

	if (distance > (bRadius* bRadius) && !inside)
	{ // if distance between two is greater than radius and the circle isn't inside, early out no collision
		return false;
	}

	distance = sqrtf(distance);

	if (inside)
	{
		collData.collisionNormal = -normal / distance;
		collData.penetration = bRadius - distance;
	}
	else
	{
		collData.collisionNormal = normal / distance;
		collData.penetration = bRadius - distance;
	}

	return true;
}

bool Krawler::Physics::CircleVsAABBCollision(KCollisionData & collData)
{
	//Setup pointers to physics bodies
	KPhysicsBody* const bodyA = collData.bodyA;
	KPhysicsBody* const bodyB = collData.bodyB;

	const KGameObject& goA = bodyA->getGameObject();
	const KGameObject& goB = bodyB->getGameObject();

	const float xExtent = goB.getFixedGlobalBounds().width / 2.0f;
	const float yExtent = goB.getFixedGlobalBounds().height / 2.0f;

	const float aRadius = goA.getFixedGlobalBounds().width / 2.0f;

	Vec2f n = (goB.getPosition() + Vec2f(xExtent, yExtent)) - (goA.getCentrePosition());
	Vec2f closest = n;

	closest.x = Clamp(-xExtent, xExtent, closest.x);
	closest.y = Clamp(-yExtent, yExtent, closest.y);

	bool inside = false;

	if (n == closest)
	{
		inside = true;
		if (fabs(n.x) > fabs(n.y))
		{
			if (closest.x > 0)
			{
				closest.x = xExtent;
			}
			else
			{
				closest.x = -xExtent;
			}
		}
		else
		{
			if (closest.y > 0)
			{
				closest.y = yExtent;
			}
			else
			{
				closest.y = -yExtent;
			}
		}
	}

	Vec2f normal = n - closest;
	//normal *= -1.f;

	float distance = GetSquareLength(normal);

	if (distance > (aRadius* aRadius) && !inside)
	{ // if distance between two is greater than radius and the circle isn't inside, early out no collision
		return false;
	}

	distance = sqrtf(distance);

	if (inside)
	{
		collData.collisionNormal = -normal / distance;
		collData.penetration = aRadius - distance;
	}
	else
	{
		collData.collisionNormal = normal / distance;
		collData.penetration = aRadius - distance;
	}

	return true;
}

bool Krawler::Physics::PolygonvsPolygon(KCollisionData & collData)
{
	collData.contactCount = 0;

	KPhysicsBody* const bodyA = collData.bodyA;
	KPhysicsBody* const bodyB = collData.bodyB;



	return true;
}
