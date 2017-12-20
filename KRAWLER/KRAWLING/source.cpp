#include "stdafx.h"
#include "vld.h"

#include <Krawler.h>
#include <KApplication.h>
#include <LogicState\KLogicStateDirector.h>
#include <LogicState\KLogicState.h>
#include <Physics\KPhysicsScene.h>
#include <TiledMap\KTiledMap.h>
#include <Utilities\KDebug.h>
#include <SLU\KStateLogicUnit.h>


#include <KEntity.h>
#include <Components\KCTransform.h>

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Input;

class TestState : public KLogicState
{
public:

	TestState() {}

	~TestState() {}

	virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser) override
	{
		auto status = KLogicState::setupState(initaliser);
		if (status != Success)
		{
			return status;
		}


		return Success;
	}

	virtual void tick() override
	{
		if (KInput::MouseJustPressed(sf::Mouse::Left))
		{

		}
		if (KInput::JustPressed(sf::Keyboard::Key::Escape))
		{
			KApplication::getApp()->closeApplication();
		}
	}

	virtual void fixedTick() override
	{

	}
};

int main(void)
{
	KApplicationInitialise initApp;
	initApp.consoleWindow = true;
	initApp.width = 640;
	initApp.height = 480;
	initApp.windowStyle = Windowed_Fixed_Size;
	initApp.gameFps = 30;
	initApp.physicsFps = 100;
	initApp.windowTitle = KTEXT("Hello World!");

	StartupEngine(&initApp);

	TestState* state = new TestState;
	auto application = KApplication::getApp();

	KLogicStateInitialiser initState;
	initState.bIsPhysicsEngineEnabled = true;

	application->getLogicStateDirector()->registerLogicState(dynamic_cast<KLogicState*>(state), &initState);
	application->getLogicStateDirector()->setActiveLogicState(initState.stateIdentifier);


	InitialiseSubmodules();

	TiledMap::KTiledMap map;
	map.setupTiledMap(KTEXT("test.dat"));
	application->getRenderer()->setActiveTiledMap(&map);
	RunApplication();
	ShutdownEngine();

	map.cleanupTiledMap();

	KEntity e;
	KEntity e2;
	e2.getComponent <Components::KCTransform>()->setParent(&e);
	e2.getComponent <Components::KCTransform>()->setScale(0.5f, 0.5f);
	auto transform = e.getComponent<Components::KCTransform>();
	transform->setScale(Vec2f(10, 10));
	transform->setRotation(100);
	transform->setTranslation(10, 10);
	transform->tick();
	e2.getComponent <Components::KCTransform>()->tick();
	auto p = transform->getTransform().getMatrix();
	auto p2 = e2.getComponent <Components::KCTransform>()->getTransform().getMatrix();
	float rota = transform->getRotation();
	float rot = e2.getComponent<Components::KCTransform>()->getRotation();
	auto scale = transform->getScale();
	auto scale2 = e2.getComponent<Components::KCTransform>()->getScale();
	system("pause");
	return 0;
}

