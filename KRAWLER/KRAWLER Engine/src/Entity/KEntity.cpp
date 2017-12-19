#include "Entity\KEntity.h"	

using namespace Krawler;

KEntity::KEntity()
	: m_componentVector(1, nullptr), m_entityTag(GenerateUUID())
{
	// pushback transform component here
}

KInitStatus KEntity::init()
{
	return KInitStatus::Success;
}

void KEntity::cleanUp()
{

}

void KEntity::tick()
{

}

void KEntity::fixedTick()
{

}

bool KEntity::addComponent(KComponentBase* pComponent)
{
	KCHECK(pComponent);
	if (!pComponent)
	{
		return false;
	}

	m_componentVector.push_back(pComponent);
	return true;
}

bool KEntity::removeComponent(KComponentBase* pComponent)
{
	auto findResult = std::find(m_componentVector.begin(), m_componentVector.end(), pComponent);

	if (findResult == m_componentVector.end())
	{
		return false;
	}
	m_componentVector.erase(findResult);
	return true;
}
