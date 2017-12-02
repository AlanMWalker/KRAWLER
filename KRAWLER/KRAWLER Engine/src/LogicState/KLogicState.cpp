
#include "LogicState\KLogicState.h"
#include "LogicState\KLogicStateDirector.h"
#include "Physics\KPhysicsScene.h"
#include "KApplication.h"

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
	mp_stateLogicAdmin = new SLU::KStateLogicUnitAdministrator(this);

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
	
	mp_stateLogicAdmin->cleanupAllUnits();

	if (mb_isPhysicsEnabled)
	{
		mp_physicsScene->cleanupScene();
		KFREE(mp_physicsScene);
	}
	mp_stateDirector = nullptr;
	KFREE(mp_stateLogicAdmin);
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
