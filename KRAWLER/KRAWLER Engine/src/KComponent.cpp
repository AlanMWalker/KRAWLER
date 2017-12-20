#include "KComponent.h"
#include "KEntity.h"

using namespace Krawler;

Krawler::KComponentBase::KComponentBase(KEntity * pEntity)
	: m_pEntity(pEntity), m_componentTag(GenerateUUID())
{
}

Krawler::KComponentBase::~KComponentBase()
{
}

KInitStatus Krawler::KComponentBase::init()
{
	return KInitStatus::Success;
}

void Krawler::KComponentBase::cleanUp()
{
}

void Krawler::KComponentBase::fixedTick()
{
}

void Krawler::KComponentBase::tick()
{
}
