#include "stdafx.h"
#include "vld.h"
#include <future>
#include <list>

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
#include <KScene.h>

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Input;
using namespace Krawler::Components;

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

	//e.addComponent(new Components::KCSprite(&e, Vec2f(100, 100)));
	//e.addComponent(new Components::KCBoxCollider(&e, Vec2f(100, 100)));
	//e.getComponent<Components::KCTransform>()->setTranslation(Vec2f(100.0f, 100.0f));
	//e.init();

	//KEntity v;
	//v.addComponent(new Components::KCSprite(&v, Vec2f(50, 50)));
	//v.addComponent(new Components::KCBoxCollider(&v, Vec2f(50, 50)));
	////v.getComponent<Components::KCTransform>()->setTranslation(Vec2f(20.0f, 5.0f));
	//v.getComponent<Components::KCTransform>()->setOrigin(Vec2f(25, 25));
	////v.getComponent<Components::KCTransform>()->rotate(10);
	////v.getComponent<Components::KCTransform>()->setParent(&e);
	//v.init();
	//v.getComponent<Components::KCSprite>()->setColour(Colour::Magenta);
	srand((unsigned)time(NULL));
	//auto mode = sf::VideoMode::getFullscreenModes()[0];
	auto mode = sf::VideoMode(640, 480);

	//qtree.retrieve(ent, &entities[4]);

	sf::RenderWindow rw;
	rw.setActive(true);
	rw.create(mode, "Testing ECS", sf::Style::Close);

	auto callback = [](KEntity* pEntity) -> void
	{
		static int x = 0;
		KPrintf(L"oo a collision! %d\n", ++x);
		//DoMe(pEntity);
	};
	KScene scene(std::wstring(KTEXT("SceneA")), Rectf(0.0f, 0.0f, mode.width, mode.height));
	std::vector<KEntity*> allocd;
	for (int i = 0; i < 10; ++i)
	{
		KEntity* pEntity = scene.addEntityToScene();
		allocd.push_back(pEntity);
		allocd.back()->addComponent(new KCSprite(pEntity, Vec2f(10, 10)));
		allocd.back()->addComponent(new KCBoxCollider(pEntity, Vec2f(10, 10)));
	}

	allocd[0]->getComponent<KCBoxCollider>()->subscribeCollisionCallback(callback);

	scene.initScene();

	for (auto& ent : allocd)
	{
		ent->getComponent<KCTransform>()->setTranslation(Vec2f(rand() % mode.width, rand() % mode.height));
	}


	KInput::SetWindow(&rw);
	rw.setVerticalSyncEnabled(true);
	while (rw.isOpen())
	{
		sf::Event evnt;
		while (rw.pollEvent(evnt))
		{
			if (evnt.type == evnt.Closed)
			{
				rw.close();
			}
			if (evnt.type == evnt.KeyPressed)
			{
				if (evnt.key.code == KKey::Escape)
				{
					rw.close();
				}
			}
			KInput::HandleEvent(evnt);
		}
		allocd[0]->getComponent<KCTransform>()->setTranslation(KInput::GetMouseWorldPosition());
		scene.tick();
		scene.fixedTick();
		rw.clear();
		for (auto& ent : allocd)
		{
			auto pSprite = ent->getComponent<KCSprite>();
			if (pSprite)
			{
				rw.draw(*pSprite);
			}
		}
		rw.display();
		KInput::Update();
	}
	scene.cleanUpScene();
	return 0;
}

