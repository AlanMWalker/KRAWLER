#include "KComponent.h"
#include "KEntity.h"

using namespace Krawler;

KComponentBase::KComponentBase(KEntity * pEntity)
	: m_pEntity(pEntity), m_componentTag(GenerateUUID())
{
}
KComponentBase::~KComponentBase()
{
}

KInitStatus KComponentBase::init()
{
	return KInitStatus::Success;
}

void KComponentBase::cleanUp()
{
}

void KComponentBase::fixedTick()
{
}

void KComponentBase::tick()
{
}

void KComponentBase::onEnterScene()
{
}
void KComponentBase::onExitScene()
{
}
