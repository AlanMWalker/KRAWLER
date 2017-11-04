#include "stdafx.h"
#include "vld.h"

#include <Krawler.h>
#include <KApplication.h>
#include <LogicState\KLogicStateDirector.h>
#include <LogicState\KLogicState.h>
#include <Physics\KPhysicsScene.h>
#include <Utilities\KDebug.h>

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Input;

class Friendy : public KLogicState
{
public:
	Friendy()
	{
	}
	~Friendy() {}

	Krawler::Physics::KPhysicsBody* body = nullptr;

	virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser) override
	{
		auto status = KLogicState::setupState(initaliser);
		if (status != Success)
		{
			return status;
		}

		mp_physicsScene->setGravity(Vec2f(0.0f, 9.81));

		auto a = addGameObject(Vec2f(1024, 10));
		a->setPosition(0.0f, 700);
		body = mp_physicsScene->addBody(a, 10000.0f, false);
		a->setPhysicsBody(body);

		return Success;
	}

	virtual void tick() override
	{
		body->resetForce();
		if(KInput::MouseJustPressed(sf::Mouse::Left))
		{
			Vec2f pos(sf::Mouse::getPosition(*KApplication::getApplicationInstance()->getRenderWindow()));
			KGameObject* a = addGameObject(Vec2f(10, 10));
			a->setPosition(pos);
			auto kpBody = mp_physicsScene->addBody(a, 1.0f);
			a->setPhysicsBody(kpBody);
			Krawler::KPrintf(KTEXT("%d bodies in scene\n"), mp_physicsScene->getPhysicsBodyCount());
		}
	}
};

int main(void)
{
	KApplicationInitialise initApp;
	initApp.consoleWindow = true;
	initApp.width = 1024;
	initApp.height = 720;
	initApp.windowStyle = Windowed_Fixed_Size;
	initApp.gameFps = 60;
	initApp.physicsFps = 100;
	initApp.windowTitle = KTEXT("Hello World!");

	StartupEngine(&initApp);

	Friendy* state = new Friendy;
	auto application = KApplication::getApplicationInstance();

	KLogicStateInitialiser initState;
	initState.bIsPhysicsEngineEnabled = true;

	application->getLogicStateDirector()->registerLogicState(dynamic_cast<KLogicState*>(state), &initState);
	application->getLogicStateDirector()->setActiveLogicState(initState.stateIdentifier);

	InitialiseStateDirector();

	RunApplication();
	ShutdownEngine();

	return 0;
}

