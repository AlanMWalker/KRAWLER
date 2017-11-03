	
#include "LogicState\KLogicStateDirector.h"

#include <algorithm>

using namespace Krawler;
using namespace Krawler::LogicState;

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
	m_currentState.first = TEXT("state");
	m_currentState.second = nullptr;
}

void Krawler::LogicState::KLogicStateDirector::tickActiveLogicState()
{
	m_currentState.second->tick();
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
	for (auto& state : m_logicStateMap)
	{
		KFREE(state.second);
	}
	m_logicStateMap.clear();
}

bool Krawler::LogicState::KLogicStateDirector::doesLogicStateExist(const std::wstring & identifier) const
{
	bool exists = m_logicStateMap.find(identifier) != m_logicStateMap.end();
	return exists;
}
