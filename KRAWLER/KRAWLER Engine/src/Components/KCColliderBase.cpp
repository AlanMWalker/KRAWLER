#include "Components\KCColliderBase.h"
#include "Physics\b2dConversion.h"
#include "Components\KCTransform.h"
#include "box2d\box2d.h"

using namespace Krawler;
using namespace Krawler::Components;

using namespace std;

//--Components::KCColliderBase--\\ 
KCColliderBase::KCColliderBase(KEntity* pEntity, KCColliderType type)
	: KComponentBase(pEntity), m_colliderType(type), m_collisionLayer(0)
{
	switch (type)
	{
		//case KCColliderType::Polygon:
	case KCColliderType::AABB:
		m_pShape = make_shared<b2PolygonShape>();
		break;
	case KCColliderType::Circle:
		m_pShape = make_shared<b2CircleShape>();
		break;
		//case KCColliderType::OBB:
		//	KPRINTF("OBB no longer a primitive type, consider using KCPolyCollider to achieve the same result\n");
		//	break;
	default:
		KPRINTF("Unexpected behaviour within KCColliderBase ctor\n");
	}
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

bool KCColliderBase::isCallbackSubscribed(KCColliderBaseCallback* callback) const
{
	auto findResult = std::find(m_callbacks.begin(), m_callbacks.end(), callback);

	return findResult != m_callbacks.end();
}

void KCColliderBase::setCollisionFilteringData(const KCColliderFilteringData& filteringPOD)
{
	m_filterData = filteringPOD;
}

b2Transform KCColliderBase::getB2Transform()
{
	const float angleInRad = Maths::Radians(getEntity()->getTransform()->getRotation());
	const b2Vec2 position = Vec2fTob2(getEntity()->getTransform()->getTranslation());
	return b2Transform(position, b2Rot(angleInRad));
}

