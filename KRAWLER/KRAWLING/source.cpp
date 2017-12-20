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
#include <Components\KCBoxCollider.h>

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
	e.addComponent(new Components::KCBoxCollider(&e, Vec2f(50, 50)));
	e.getComponent<Components::KCTransform>()->setTranslation(Vec2f(100.0f, 100.0f));
	e.init();

	KEntity v;
	v.addComponent(new Components::KCSprite(&v, Vec2f(100, 100)));
	v.addComponent(new Components::KCBoxCollider(&v, Vec2f(100, 100)));
	v.getComponent<Components::KCTransform>()->setTranslation(Vec2f(20.0f, 5.0f));
	v.getComponent<Components::KCTransform>()->setOrigin(Vec2f(50.0f, 50.0f));
	//v.getComponent<Components::KCTransform>()->rotate(10);
	v.getComponent<Components::KCTransform>()->setParent(&e);
	v.init();
	v.getComponent<Components::KCSprite>()->setColour(Colour::Magenta);
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
		Components::KCTransform* pTrans = v.getComponent<Components::KCTransform>();
		float dt = 0.016f;

		if (KInput::Pressed(KKey::D))
		{
			pTrans->move(50.0f * dt, 0.0f);
		}

		if (KInput::Pressed(KKey::A))
		{
			pTrans->move(-50.0f * dt, 0.0f);
		}

		if (KInput::Pressed(KKey::W))
		{
			pTrans->move(0.0f, -50.0f*dt);
		}

		if (KInput::Pressed(KKey::S))
		{
			pTrans->move(0.0f, 50.0f*dt);
		}

		if (KInput::Pressed(KKey::Q))
		{
			pTrans->rotate(-50.0f*dt);
		}


		if (KInput::Pressed(KKey::E))
		{
			pTrans->rotate(50.0f*dt);
		}
		e.tick();
		v.tick();

		if (e.getComponent<Components::KCBoxCollider>()->checkIntersects(v.getComponent<Components::KCBoxCollider>()))
		{
			e.getComponent<Components::KCSprite>()->setColour(Colour::Red);
		}
		else
		{
			e.getComponent<Components::KCSprite>()->setColour(Colour::Blue);

		}

		rw.clear();
		auto pE = e.getComponent<Components::KCSprite>();
		auto pV = v.getComponent<Components::KCSprite>();

		rw.draw(*pE);
		rw.draw(*pV);
		rw.display();
		KInput::Update();
	}
	e.cleanUp();
	v.cleanUp();
	return 0;
}

