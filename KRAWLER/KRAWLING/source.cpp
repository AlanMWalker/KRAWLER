#include "stdafx.h"
#include "vld.h"
#include <future>
#include <list>

#include <Krawler.h>
#include <KApplication.h>
#include <Physics\KPhysicsScene.h>
#include <TiledMap\KTiledMap.h>
#include <Utilities\KDebug.h>


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

class CustomComponent : public KComponentBase
{
public:
	CustomComponent(KEntity* pEntity)
		: KComponentBase(pEntity)
	{

	}

	~CustomComponent() = default;

	virtual void tick()
	{
		KComponentBase::tick();
		float dt = KApplication::getApp()->getDeltaTime();
		KCTransform* transform = getEntity()->getComponent<KCTransform>();

		if (KInput::Pressed(KKey::W))
		{
			transform->move(0.0f, -10.0f * dt);
		}

		if (KInput::Pressed(KKey::S))
		{
			transform->move(0.0f, 10.0f * dt);
		}

	}
private:

};

int main(void)
{
	srand((unsigned)time(NULL));

	//KApplicationInitialise initApp;
	//initApp.consoleWindow = true;
	//initApp.width = 640;
	//initApp.height = 480;
	//initApp.windowStyle = Windowed_Fixed_Size;
	//initApp.gameFps = 30;
	//initApp.physicsFps = 100;
	//initApp.windowTitle = KTEXT("Hello World!");
	//
	KApplicationInitialise initApp(true);
	StartupEngine(&initApp);

	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0.0f, 0.0f, initApp.width, initApp.height)));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));

	auto pCurrentScene = app->getSceneDirector().getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	auto entity2 = pCurrentScene->addEntityToScene();
	entity->addComponent(new KCSprite(entity, Vec2f(64, 64)));
	entity2->addComponent(new KCSprite(entity2, Vec2f(64, 64)));
	entity->addComponent(new CustomComponent(entity));
	InitialiseSubmodules();

	entity->getComponent<KCTransform>()->setTranslation(10.0f, 10.0f);
	entity2->getComponent<KCTransform>()->setTranslation(10.0f, 10.0f);
	entity2->getComponent<KCSprite>()->setColour(Colour::Green);
	entity2->getComponent<KCSprite>()->setRenderLayer(-2);
	entity2->getComponent<KCTransform>()->setParent(entity);

	RunApplication();

	ShutdownEngine();

	return 0;
}

