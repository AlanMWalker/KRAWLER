#include "Components\KCColliderBase.h"

#include "Components\KCBoxCollider.h"
#include "Components\KCCircleCollider.h"

#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

using namespace std;

bool Krawler::AABBvsAABB(KCollisionDetectionData & collData)
{
	//Setup pointers to physics bodies
	KCBoxCollider* const colliderEntityA = collData.entityA->getComponent<KCBoxCollider>();
	KCBoxCollider* const colliderEntityB = collData.entityB->getComponent<KCBoxCollider>();

	KCTransform* const transformA = collData.entityA->getComponent<KCTransform>();
	KCTransform* const transformB = collData.entityB->getComponent<KCTransform>();

	Vec2f topLeftA, topLeftB; // top left co-ord of the entity
	topLeftA = colliderEntityA->getTopLeftCoord();
	topLeftB = colliderEntityB->getTopLeftCoord();

	const Vec2f halfBoundsA(colliderEntityA->getHalfSize());
	const Vec2f halfBoundsB(colliderEntityB->getHalfSize());

	Vec2f n;
	n = (topLeftB + halfBoundsB) - (topLeftA + halfBoundsA);
	float xOverlap = halfBoundsA.x + halfBoundsB.x - fabs(n.x);

	if (xOverlap > 0.0f)
	{
		float yOverlap = halfBoundsA.y + halfBoundsB.y - fabs(n.y);

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

bool Krawler::CirclevsCircle(KCollisionDetectionData& collData)
{
	//Setup pointers to physics bodies
	KCCircleCollider* const pCircleColliderA = collData.entityA->getComponent<KCCircleCollider>();
	KCCircleCollider* const pCircleColliderB = collData.entityB->getComponent<KCCircleCollider>();

	const float aRadius = pCircleColliderA->getRadius();
	const float bRadius = pCircleColliderB->getRadius();;

	const Vec2f aCentre = pCircleColliderA->getCentrePosition();
	const Vec2f bCentre = pCircleColliderB->getCentrePosition();

	Vec2f n = bCentre - aCentre; // Direction from b to a 

	float sumofRadii = aRadius + bRadius;  // calculate the sum of radius a and radius b


	if (GetSquareLength(n) > (sumofRadii*sumofRadii)) //if distance between circles > sum of radii, no collision
	{
		return false;
	}

	float distance = GetLength(n);

	if (distance != 0)
	{
		//penetration distance is the difference between radial sum & actual distance
		collData.penetration = sumofRadii - distance;
		collData.collisionNormal = (bCentre - aCentre) / distance;
	}
	else
	{
		collData.penetration = aRadius;
		collData.collisionNormal = Vec2f(1.0f, 0.0f);
	}

	return true;
}

bool Krawler::AABBvsCircle(KCollisionDetectionData & collData)
{
	//Setup pointers to physics bodies
	KCBoxCollider* const colliderEntityA = collData.entityA->getComponent<KCBoxCollider>();
	KCCircleCollider* const colliderEntityB = collData.entityB->getComponent<KCCircleCollider>();

	KCTransform* pTransformEntityA = collData.entityA->getComponent<KCTransform>();

	const float xExtent = colliderEntityA->getHalfSize().x;
	const float yExtent = colliderEntityA->getHalfSize().y;

	const float bRadius = colliderEntityB->getRadius();

	Vec2f entityACentre = colliderEntityA->getTopLeftCoord() + colliderEntityA->getHalfSize(); //top left coord of A

	Vec2f n = (colliderEntityB->getCentrePosition()) - (entityACentre);
	Vec2f closest = n;

	closest.x = Maths::Clamp(-xExtent, xExtent, closest.x);
	closest.y = Maths::Clamp(-yExtent, yExtent, closest.y);

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

bool Krawler::CirclevsAABB(KCollisionDetectionData & collData)
{
	KCCircleCollider* const colliderEntityA = collData.entityA->getComponent<KCCircleCollider>();
	KCBoxCollider* const colliderEntityB = collData.entityB->getComponent<KCBoxCollider>();

	KCTransform* pTransformEntityA = collData.entityA->getComponent<KCTransform>();

	const float xExtent = colliderEntityB->getHalfSize().x;
	const float yExtent = colliderEntityB->getHalfSize().y;

	const float bRadius = colliderEntityA->getRadius();

	Vec2f entityACentre = colliderEntityB->getTopLeftCoord() + colliderEntityB->getHalfSize(); //top left coord of A

	Vec2f n = (colliderEntityA->getCentrePosition()) - (entityACentre);
	Vec2f closest = n;

	closest.x = Maths::Clamp(-xExtent, xExtent, closest.x);
	closest.y = Maths::Clamp(-yExtent, yExtent, closest.y);

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
	collData.collisionNormal *= -1.0f;

	return true;
}

//--Components::KCColliderBase--\\ 
KCColliderBase::KCColliderBase(KEntity * pEntity, KCColliderType type)
	: KComponentBase(pEntity), m_colliderType(type)
{
}

int32 KCColliderBase::subscribeCollisionCallback(KCColliderBaseCallback* callback)
{
	KCHECK(callback);
	if (!callback)
	{
		return -1; // if callback passed is null, return -1 to indicarte it wasn't added
	}

	m_callbacks.push_back(callback);
	return 0;
}

int32 KCColliderBase::unsubscribeCollisionCallback(KCColliderBaseCallback* callback)
{
	auto findResult = std::find(m_callbacks.begin(), m_callbacks.end(), callback);

	if (findResult == m_callbacks.end())
	{
		return -1; // if this call back isn't subscribed to the collider then return -1
	}

	m_callbacks.erase(findResult); // remove the callback from the vector
	return 0;
}

void KCColliderBase::collisionCallback(const KCollisionDetectionData& collData)
{
	for (auto& callback : m_callbacks)
	{
		(*callback)(collData);
	}
}

KRAWLER_API bool Krawler::Components::KCColliderBase::isCallbackSubscribed(KCColliderBaseCallback * callback) const
{
	auto findResult = std::find(m_callbacks.begin(), m_callbacks.end(), callback);

	return findResult != m_callbacks.end();
}
