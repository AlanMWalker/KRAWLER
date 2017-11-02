#include "stdafx.h"
#include "LogicState\KLogicState.h"
#include "LogicState\KLogicStateDirector.h"

using namespace Krawler;
using namespace Krawler::LogicState;

using namespace std;

KInitStatus KLogicState::setupState(const KLogicStateInitialiser& initaliser)
{
	m_stateIdentifier = initaliser.stateIdentifier;
	mb_isPhysicsEnabled = initaliser.bIsPhysicsEngineEnabled;
	mp_stateDirector = initaliser.pStateDirector;

	return KInitStatus::Success;
}

void KLogicState::cleanupState()
{
}

void KLogicState::tick()
{
}
