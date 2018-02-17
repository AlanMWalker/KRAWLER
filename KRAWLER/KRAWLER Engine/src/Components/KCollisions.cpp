#include <Components\KCollisions.h>

#include "Components\KCBoxCollider.h"
#include "Components\KCCircleCollider.h"
#include <Components\KCColliderBase.h>
#include <Components\KCOrientedBoxCollider.h>

#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

//SAT Helper functions ( currently for OBB impl only) 
static float FindAxisLeastPenetration(int32& faceIdx, KCOrientedBoxCollider* const colA, KCOrientedBoxCollider* const colB);

static __forceinline bool BiasGreaterThan(float a, float b)
{
	const float k_biasRelative = 0.95f;
	const float k_biasAbsolute = 0.01f;
	return a >= b * k_biasRelative + a * k_biasAbsolute;
}

static void FindIncidentFace(Vec2f v[], KCOrientedBoxCollider* const refOBB, KCOrientedBoxCollider* const incOBB, int32 refIdx);

static int32 clip(Vec2f n, float c, Vec2f face[]);

bool Krawler::AABBvsAABB(KCollisionDetectionData & collData)
{
	//Setup pointers to physics bodies
	KCBoxCollider* const colliderEntityA = collData.entityA->getComponent<KCBoxCollider>();
	KCBoxCollider* const colliderEntityB = collData.entityB->getComponent<KCBoxCollider>();

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

	const Vec2f& aCentre = pCircleColliderA->getCentrePosition();
	const Vec2f& bCentre = pCircleColliderB->getCentrePosition();

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

bool Krawler::OBBvsAABB(KCollisionDetectionData & data)
{
	return true;
}

bool Krawler::AABBvsOBB(KCollisionDetectionData & data)
{
	return true;
}

bool Krawler::OBBvsCircle(KCollisionDetectionData & collData)
{
	KEntity* temp = collData.entityA;
	collData.entityA = collData.entityB;
	collData.entityB = temp;

	bool result = CirclevsOBB(collData);

	temp = collData.entityA;
	collData.entityA = collData.entityB;
	collData.entityB = temp;
	collData.collisionNormal = -collData.collisionNormal;

	return result;
}

bool Krawler::CirclevsOBB(KCollisionDetectionData & collData)
{
	constexpr float EPSILON = 0.0001f;

	KCCircleCollider* circleCollider = collData.entityA->getComponent<KCCircleCollider>();
	KCOrientedBoxCollider* obbCollider = collData.entityB->getComponent<KCOrientedBoxCollider>();

	if (!circleCollider || !obbCollider)
	{
		return false;
	}

	const KOrientation& obbOrientation = obbCollider->getOrientation();

	collData.contactCount = 0;

	Vec2f centre = circleCollider->getCentrePosition();
	const float radius = circleCollider->getRadius();
	//transform circle centre to obb space
	centre = obbOrientation.getTransposedOrientation() * (centre - obbCollider->getPosition());

	float separation = -FLT_MAX;
	int32 faceNormal = 0;

	for (int32 i = 0; i < FaceCount; ++i)
	{
		float s = DotProduct(obbCollider->getNormal(i), centre - obbCollider->getVertexLocalPosition(i));

		if (s > radius)
		{
			return false;
		}

		if (s > separation)
		{
			separation = s;
			faceNormal = i;
		}
	}

	Vec2f v1 = obbCollider->getVertexLocalPosition(faceNormal);
	int32 faceNormal2 = (faceNormal + 1 >= FaceCount) ? 0 : faceNormal + 1;
	Vec2f v2 = obbCollider->getVertexLocalPosition(faceNormal2);

	//if centre is within obb
	if (separation < EPSILON)
	{
		collData.contactCount = 1;
		collData.collisionNormal = obbCollider->getOrientedFaceNormal(faceNormal);
		collData.contacts[0] = collData.collisionNormal * radius + circleCollider->getCentrePosition();
		collData.penetration = radius;
		return true;
	}

	const float dot1 = DotProduct(centre - v1, v2 - v1);
	const float dot2 = DotProduct(centre - v2, v1 - v2);

	collData.penetration = (radius - separation) / radius;

	if (dot1 <= 0.0f)
	{
		if (GetSquareLength(centre - v1) > radius * radius)
		{
			return false;
		}
		collData.contactCount = 1;
		Vec2f n = v1 - centre;
		n = obbOrientation* n;
		n = Normalise(n);
		collData.collisionNormal = n;
		collData.contacts[0] = obbOrientation * v1 + obbCollider->getPosition();
	}
	else if (dot2 <= 0.0f)
	{
		if (GetSquareLength(centre - v2) > radius * radius)
		{
			return false;
		}

		collData.contactCount = 1;
		Vec2f n = v2 - centre;
		v2 = obbOrientation*n;
		n = Normalise(n);
		collData.collisionNormal = n;
		collData.contacts[0] = obbOrientation * v2 + obbCollider->getPosition();
	}
	else
	{
		Vec2f n = obbCollider->getNormal(faceNormal);
		if (DotProduct(centre - v1, n) > radius)
		{
			return false;
		}
		n = obbOrientation*n;
		collData.collisionNormal = -n;
		collData.contacts[0] = collData.collisionNormal * radius + circleCollider->getCentrePosition();
		collData.contactCount = 1;
	}

	return true;
}

bool Krawler::OBBvsOBB(KCollisionDetectionData & collData)
{
	KCOrientedBoxCollider *obb_A, *obb_B;
	obb_A = collData.entityA->getComponent<KCOrientedBoxCollider>();
	obb_B = collData.entityB->getComponent<KCOrientedBoxCollider>();

	if (!obb_A || !obb_B)
	{
		return false;
	}

	int32 faceA;
	float penA = FindAxisLeastPenetration(faceA, obb_A, obb_B);
	if (penA >= 0.0f)
	{
		return false;
	}

	int32 faceB;
	float penB = FindAxisLeastPenetration(faceB, obb_B, obb_A);
	if (penB >= 0.0f)
	{
		return false;
	}

	uint32 refIndex;
	bool flip; //always point from a to b

	KCOrientedBoxCollider *refOBB, *incOBB; //reference and incident 

	if (BiasGreaterThan(penA, penB))
	{
		refOBB = obb_A;
		incOBB = obb_B;
		refIndex = faceA;
		flip = false;
	}
	else
	{
		refOBB = obb_B;
		incOBB = obb_A;
		refIndex = faceB;
		flip = true;
	}

	Vec2f incidentFace[2];
	FindIncidentFace(incidentFace, refOBB, incOBB, refIndex);

	//get face vertices from incident face
	Vec2f v1 = refOBB->getVertexWorldPosition(refIndex);
	refIndex = (refIndex + 1 >= FaceCount) ? 0 : refIndex + 1;
	Vec2f v2 = refOBB->getVertexWorldPosition(refIndex);

	//calculate face side normal in world space
	Vec2f sidePlaneNormal = Normalise((v2 - v1));

	Vec2f refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

	float refC = DotProduct(refFaceNormal, v1);
	float negativeSide = -DotProduct(sidePlaneNormal, v1);
	float posSide = DotProduct(sidePlaneNormal, v2);

	if (clip(-sidePlaneNormal, negativeSide, incidentFace) < 2)
	{
		return false;
	}

	if (clip(sidePlaneNormal, posSide, incidentFace) < 2)
	{
		return false;
	}

	collData.collisionNormal = flip ? -refFaceNormal : refFaceNormal;

	uint32 cp = 0;
	float separation = DotProduct(refFaceNormal, incidentFace[0]) - refC;

	if (separation <= 0.0f)
	{
		collData.contacts[cp] = incidentFace[0];
		collData.penetration = -separation;
		++cp;
	}
	else
	{
		collData.penetration = 0.0f;
	}

	separation = DotProduct(refFaceNormal, incidentFace[1]) - refC;
	if (separation <= 0.0f)
	{
		collData.contacts[cp] = incidentFace[1];
		collData.penetration += -separation;
		++cp;

		collData.penetration /= (float)cp;
	}

	collData.contactCount = cp;

	return true;
}

float FindAxisLeastPenetration(int32 & faceIdx, KCOrientedBoxCollider * const colA, KCOrientedBoxCollider * const colB)
{
	float bestDistance = -FLT_MAX;
	uint32 bestIdx = 0;

	for (int32 i = 0; i < FaceCount; ++i)
	{
		Vec2f orientedNormal = colA->getOrientedFaceNormal(i); // normal which accounts for collider a's rotation

		KOrientation transposedOrientationB = colB->getOrientation().getTransposedOrientation();

		Vec2f normal = transposedOrientationB * orientedNormal;

		//retrieve support point from B along -normal
		Vec2f s = colB->getSupport(-normal);

		Vec2f v = colA->getVertexWorldPosition(i);
		v -= colB->getPosition();
		v = transposedOrientationB * v;

		float dp = DotProduct(normal, s - v);

		if (dp > bestDistance)
		{
			bestDistance = dp;
			bestIdx = i;
		}
	}

	faceIdx = bestIdx;
	return bestDistance;
}

void FindIncidentFace(Vec2f v[], KCOrientedBoxCollider * const refOBB, KCOrientedBoxCollider * const incOBB, int32 refIdx)
{
	Vec2f refNormal = refOBB->getOrientedFaceNormal(refIdx);

	refNormal = incOBB->getOrientation().getTransposedOrientation() *  refNormal;

	int32 incFace = 0;
	float minDP = FLT_MAX;
	for (int32 i = 0; i < FaceCount; ++i)
	{
		float dp = DotProduct(refNormal, incOBB->getNormal(i));
		if (dp < minDP)
		{
			minDP = dp;
			incFace = i;
		}
	}

	v[0] = incOBB->getVertexWorldPosition(incFace);
	incFace = (incFace + 1 >= FaceCount) ? 0 : incFace + 1;
	v[1] = incOBB->getVertexWorldPosition(incFace);
}

int32 clip(Vec2f n, float c, Vec2f face[])
{
	int32 sp = 0;
	Vec2f out[2] =
	{
		face[0],
		face[1]
	};

	float d1 = DotProduct(n, face[0]) - c;
	float d2 = DotProduct(n, face[1]) - c;

	if (d1 <= 0.0f)
	{
		out[sp++] = face[0];
	}

	if (d2 <= 0.0f)
	{
		out[sp++] = face[1];
	}

	if (d1*d2 < 0.0f)
	{
		float alpha = d1 / (d1 - d2);
		out[sp] = face[0] + alpha * (face[1] - face[0]);
		++sp;
	}

	face[0] = out[0];
	face[1] = out[1];

	KCHECK(sp != 3);
	return sp;
}
