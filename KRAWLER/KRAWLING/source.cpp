#include "stdafx.h"
#include "vld.h"
#include <future>

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
#include <Components\KCSprite.h>

#include <SFML\Graphics.hpp>

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
	//KApplicationInitialise initApp;
	//initApp.consoleWindow = true;
	//initApp.width = 640;
	//initApp.height = 480;
	//initApp.windowStyle = Windowed_Fixed_Size;
	//initApp.gameFps = 30;
	//initApp.physicsFps = 100;
	//initApp.windowTitle = KTEXT("Hello World!");
	//
	//StartupEngine(&initApp);
	//
	//TestState* state = new TestState;
	//auto application = KApplication::getApp();
	//
	//KLogicStateInitialiser initState;
	//initState.bIsPhysicsEngineEnabled = true;
	//
	//application->getLogicStateDirector()->registerLogicState(dynamic_cast<KLogicState*>(state), &initState);
	//application->getLogicStateDirector()->setActiveLogicState(initState.stateIdentifier);
	//
	//
	//InitialiseSubmodules();
	//
	//TiledMap::KTiledMap map;
	//map.setupTiledMap(KTEXT("test.dat"));
	//application->getRenderer()->setActiveTiledMap(&map);
	//RunApplication();
	//ShutdownEngine();
	//
	//map.cleanupTiledMap();

	KEntity e;
	e.addComponent(new Components::KCSprite(&e, Vec2f(100, 100)));
	e.getComponent<Components::KCTransform>()->setTranslation(Vec2f(100.0f, 100.0f));

	e.init();

	sf::RenderWindow rw;
	rw.setActive(true);
	rw.create(sf::VideoMode(640, 480), "Testing ECS");
	KInput::SetWindow(&rw);
	rw.setVerticalSyncEnabled(true);
	while (rw.isOpen())
	{
		sf::Event evnt;
		while (rw.pollEvent(evnt))
		{
			if (evnt.type == evnt.Closed)
				rw.close();
			KInput::HandleEvent(evnt);
		}
		if (KInput::Pressed(KKey::Right))
		{
			e.getComponent<Components::KCTransform>()->move(50.0f * 0.016f, 0.0f);
		}
		e.tick();
		rw.clear();
		auto p = e.getComponent<Components::KCSprite>();

		rw.draw(*p);
		rw.display();
		KInput::Update();
	}
	e.cleanUp();

	system("pause");
	return 0;
}

