#include "Components\KCColliderBase.h"

#include "Components\KCBoxCollider.h"
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

using namespace std;

bool Krawler::AABBvsAABB(KCollisionDetectionData & collData)
{
	//Setup pointers to physics bodies
	KCBoxCollider* const bodyA = collData.entityA->getComponent<KCBoxCollider>();
	KCBoxCollider* const bodyB = collData.entityB->getComponent<KCBoxCollider>();

	KCTransform* const transformA = collData.entityA->getComponent<KCTransform>();
	KCTransform* const transformB = collData.entityB->getComponent<KCTransform>();

	Vec2f topLeftA, topLeftB; // top left co-ord of the entity

	topLeftA = transformA->getOrigin();
	topLeftB = transformB->getOrigin();

	topLeftA = -(Vec2f(topLeftA.x * transformA->getScale().x, topLeftA.y * transformA->getScale().y));
	topLeftB = -(Vec2f(topLeftB.x * transformB->getScale().x, topLeftB.y * transformB->getScale().y));

	topLeftA = transformA->getTransform().transformPoint(topLeftA);
	topLeftB = transformB->getTransform().transformPoint(topLeftB);

	const Vec2f halfBoundsA(bodyA->getHalfSize());
	const Vec2f halfBoundsB(bodyB->getHalfSize());

	Vec2f n;
	n = (topLeftB + halfBoundsB) - (topLeftA + halfBoundsA);

	//float aExtent = goA.getFixedGlobalBounds().width / 2.0f;
	//float bExtent = goB.getFixedGlobalBounds().width / 2.0f;

	float xOverlap = halfBoundsA.x + halfBoundsB.x - fabs(n.x);

	if (xOverlap > 0.0f)
	{
		//float aExtent = goA.getFixedGlobalBounds().height / 2.0f;
		//float bExtent = goB.getFixedGlobalBounds().height / 2.0f;

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
