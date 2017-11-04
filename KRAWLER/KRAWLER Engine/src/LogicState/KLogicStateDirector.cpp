
#include "LogicState\KLogicStateDirector.h"

#include <algorithm>

using namespace Krawler;
using namespace Krawler::LogicState;

KInitStatus Krawler::LogicState::KLogicStateDirector::initaliseLogicStates()
{
	for (auto& ls : m_logicStateMap)
	{
		if (m_logicStateInitialisers.find(ls.first) == m_logicStateInitialisers.end())
		{
			return KInitStatus::Failure;
		}
		KINIT_CHECK(ls.second->setupState(m_logicStateInitialisers[ls.first]));
		if (status != Success)
		{
			return status;
		}
	}

	m_logicStateInitialisers.clear();
	return Success;
}

void Krawler::LogicState::KLogicStateDirector::registerLogicState(KLogicState * plogicState, KLogicStateInitialiser * pInitialiser)
{
	KCHECK(plogicState);
	KCHECK(pInitialiser);

	if (!plogicState || !pInitialiser)
	{
		return;
	}

	KLogicStateData data;
	data.first = pInitialiser->stateIdentifier;
	data.second = plogicState;

	m_logicStateMap.emplace(data);
	m_logicStateInitialisers.emplace(std::pair<std::wstring, KLogicStateInitialiser>(pInitialiser->stateIdentifier, *pInitialiser));

	m_currentState.first = TEXT("state");
	m_currentState.second = nullptr;
}

void Krawler::LogicState::KLogicStateDirector::tickActiveLogicState()
{
	m_currentState.second->tick();
}

void Krawler::LogicState::KLogicStateDirector::fixedTick()
{
	m_currentState.second->fixedTick();
}

void Krawler::LogicState::KLogicStateDirector::setActiveLogicState(const std::wstring & identifier)
{
	if (doesLogicStateExist(identifier))
	{
		m_currentState.first = identifier;
		m_currentState.second = m_logicStateMap[identifier];
	}
}

void Krawler::LogicState::KLogicStateDirector::cleanupLogicStateDirector()
{
	for (auto state : m_logicStateMap)
	{	
		state.second->cleanupState();
		KFREE(state.second);
	}
	
	m_logicStateMap.clear();
}

bool Krawler::LogicState::KLogicStateDirector::doesLogicStateExist(const std::wstring & identifier) const
{
	bool exists = m_logicStateMap.find(identifier) != m_logicStateMap.end();
	return exists;
}
