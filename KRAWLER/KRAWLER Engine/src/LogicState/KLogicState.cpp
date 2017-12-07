
#include "LogicState\KLogicState.h"
#include "LogicState\KLogicStateDirector.h"
#include "Physics\KPhysicsScene.h"
#include "KApplication.h"
#include <algorithm>

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Physics;

using namespace std;

KLogicState::~KLogicState()
{

}

KInitStatus KLogicState::setupState(const KLogicStateInitialiser& initaliser)
{
	m_stateIdentifier = initaliser.stateIdentifier;
	mb_isPhysicsEnabled = initaliser.bIsPhysicsEngineEnabled;
	mp_stateDirector = initaliser.pStateDirector;
	mp_slAdmin = new SLU::KStateLogicUnitAdministrator(this);

	if (mb_isPhysicsEnabled)
	{
		mp_physicsScene = new KPhysicsScene;
	}

	return KInitStatus::Success;
}

void KLogicState::cleanupState()
{
	for (auto& pObj : m_gameObjects)
	{
		KFREE(pObj);
	}

	mp_slAdmin->cleanupAllUnits();

	if (mb_isPhysicsEnabled)
	{
		mp_physicsScene->cleanupScene();
		KFREE(mp_physicsScene);
	}
	mp_stateDirector = nullptr;
	mp_slAdmin->cleanupAllUnits();
	KFREE(mp_slAdmin);
}

void Krawler::LogicState::KLogicState::fixedTick()
{
	if (!mb_isPhysicsEnabled)
	{
		return;
	}
	const KApplication* const instance = KApplication::getApplicationInstance();

	KCHECK(instance);

	mp_physicsScene->step(instance->getPhysicsDelta());
}

void KLogicState::tick()
{
	mp_slAdmin->tickAllUnits();
}

KRAWLER_API KGameObject * Krawler::LogicState::KLogicState::addGameObject(const Vec2f& size, bool render)
{
	KGameObject* const pObj = new KGameObject(size);
	KCHECK(pObj);
	if (!pObj)
	{
		return nullptr;
	}
	m_gameObjects.push_back(pObj);
	if (render)
	{
		KApplication::getApplicationInstance()->getRenderer()->addToRendererQueue(pObj);
	}
	return pObj;
}

void Krawler::LogicState::KLogicState::physicsLerp(float alpha)
{
	if (!mb_isPhysicsEnabled)
	{
		return;
	}
	mp_physicsScene->LerpPositions(alpha);
}

KRAWLER_API KGameObject * Krawler::LogicState::KLogicState::getGameObjectByName(const std::wstring & name) const
{
	auto findResult = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), [&name](KGameObject* pObj) -> bool
	{
		return pObj->getObjectName() == name;
	});

	return *findResult;
}
