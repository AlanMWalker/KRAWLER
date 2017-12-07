#include "SLU\KStateLogicUnit.h"

using namespace Krawler;
using namespace Krawler::SLU;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*				KLogicUnitState Implementation				 *
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

KStateLogicUnit::KStateLogicUnit(const std::wstring& unitTag, KStateLogicUnitAdministrator& rAdmin)
	: mr_logicUnitAdmin(rAdmin), m_unitTag(unitTag)
{
}

KStateLogicUnit::~KStateLogicUnit()
{
	m_unitTag = NO_LOGIC_UNIT_TAG;
}

KInitStatus KStateLogicUnit::initialiseUnit()
{
	m_bHasBeenInitialised = true;
	return KInitStatus::Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*			KGameObjectLogicUnit Implementation				 *
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

KGameObjectLogicUnit::KGameObjectLogicUnit(const std::wstring& unitTag, KStateLogicUnitAdministrator& rAdmin)
	: KStateLogicUnit(unitTag, rAdmin), mp_gameObj(nullptr)
{
}

KGameObjectLogicUnit::~KGameObjectLogicUnit()
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*		KLogicUnitStateAdministrator Implementation			 *
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

KStateLogicUnitAdministrator::KStateLogicUnitAdministrator(LogicState::KLogicState* pLogicState)
	: mp_logicState(pLogicState)
{
}

KStateLogicUnitAdministrator::~KStateLogicUnitAdministrator()
{
}

KRAWLER_API KInitStatus KStateLogicUnitAdministrator::initAllUnits()
{
	m_bIsProcessingInit = true;
	for (auto pair : m_logicUnits)
	{
		KINIT_CHECK(pair.second->initialiseUnit());
		if (status != Success)
			return status;
	}

	for (auto& pair : m_delayedInitUnits)
	{
		KINIT_CHECK(pair.second->initialiseUnit());
		if (status != Success)
			return status;
		m_logicUnits.emplace(pair);
	}
	m_delayedInitUnits.clear();
	return KInitStatus::Success;
}

KRAWLER_API void KStateLogicUnitAdministrator::addUnit(KStateLogicUnit * pUnit)
{
	if (!m_bIsProcessingInit)
	{
		m_logicUnits.emplace(pUnit->getUnitTag(), pUnit);
	}
	else
	{
		m_delayedInitUnits.emplace(pUnit->getUnitTag(), pUnit);
	}
}

KRAWLER_API void KStateLogicUnitAdministrator::removeUnit(KStateLogicUnit * pUnit)
{
	auto pos = m_logicUnits.find(pUnit->getUnitTag());
	if (pos != m_logicUnits.end())
	{
		m_logicUnits.erase(pos);
	}
}

KRAWLER_API void KStateLogicUnitAdministrator::cleanupAllUnits()
{
	for (auto& pair : m_logicUnits)
	{
		pair.second->cleanupUnit();
		KFREE(pair.second);
	}
	m_logicUnits.clear();
}

KRAWLER_API void KStateLogicUnitAdministrator::tickAllUnits()
{
	for (auto pair : m_logicUnits)
	{
		pair.second->tickUnit();
	}
}

KRAWLER_API void KStateLogicUnitAdministrator::fixedTickAllUnits()
{
	for (auto pair : m_logicUnits)
	{
		pair.second->fixedTickUnit();
	}
}

KGameObjectLogicUnit * Krawler::SLU::KStateLogicUnitAdministrator::getGameLogicUnitByGameObjectName(const std::wstring & objectName)
{

	auto findResult = std::find_if(m_logicUnits.begin(), m_logicUnits.end(), [&objectName](const std::pair<std::wstring, KStateLogicUnit*>& entry) -> bool
	{
		KGameObjectLogicUnit* castedLogicUnit = dynamic_cast<KGameObjectLogicUnit*>(entry.second);
		if (!castedLogicUnit)
		{
			return false;
		}

		return castedLogicUnit->getGameObjectName() == objectName;
	});

	return (findResult != m_logicUnits.end()) ? dynamic_cast<KGameObjectLogicUnit*>((*findResult).second) : nullptr;
}
