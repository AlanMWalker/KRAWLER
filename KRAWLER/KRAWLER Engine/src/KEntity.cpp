#include "KEntity.h"	
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

KEntity::KEntity()
	: m_componentVector(1, nullptr), m_entityTag(GenerateUUID()), m_bIsInUse(false)
{
	m_componentVector[0] = new KCTransform(this);
}

KInitStatus KEntity::init()
{
	for (auto& pComponent : m_componentVector)
	{
		KCHECK(pComponent);
		KINIT_CHECK(pComponent->init());
		if (status != KInitStatus::Success)
		{
			return status;
		}
	}

	return KInitStatus::Success;
}

void KEntity::cleanUp()
{
	for (auto& pComp : m_componentVector)
	{
		KCHECK(pComp);

		pComp->cleanUp();
		KFREE(pComp);
	}
	m_componentVector.clear();
}

void KEntity::tick()
{
	for (auto& pComp : m_componentVector)
	{
		KCHECK(pComp);
		pComp->tick();
	}
}

void KEntity::fixedTick()
{
	for (auto& pComp : m_componentVector)
	{
		KCHECK(pComp);
		pComp->fixedTick();
	}
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
