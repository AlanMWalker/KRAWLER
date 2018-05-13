#include "KEntity.h"	
#include "Components\KCTransform.h"

using namespace Krawler;
using namespace Krawler::Components;

KEntity::KEntity()
	: m_componentVector(1, nullptr), m_entityTag(GenerateUUID()), m_bIsInUse(false)
{
	m_componentVector[0] = new KCTransform(this);
	m_pTransform = dynamic_cast<KCTransform*>(m_componentVector[0]);
}


KEntity::~KEntity()
{
	for (KComponentBase* pComponent : m_componentVector)
	{
		pComponent->cleanUp();
		KFREE(pComponent);
	}
	m_componentVector.clear();
}

KInitStatus KEntity::init()
{
	for (int32 componentIdx = 0; componentIdx < (signed)m_componentVector.size(); ++componentIdx)
	{
		KCHECK(m_componentVector[componentIdx]);
		KINIT_CHECK(m_componentVector[componentIdx]->init());
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

void KEntity::onEnterScene()
{
	for (auto& pComp : m_componentVector)
	{
		KCHECK(pComp);
		pComp->onEnterScene();
	}
}

void KEntity::onExitScene()
{
	for (auto& pComp : m_componentVector)
	{
		KCHECK(pComp);
		pComp->onExitScene();
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
