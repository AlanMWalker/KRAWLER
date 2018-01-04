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
#include <Components\KCPhysicsBody.h>

#include <SFML\Graphics.hpp>
#include <KScene.h>

using namespace Krawler;
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

	virtual KInitStatus init() override
	{
		auto pCurrentScene = KApplication::getApp()->getCurrentScene();

		auto floor = pCurrentScene->addEntityToScene();
		floor->setEntityTag(KTEXT("floor"));
		Vec2f floorSize(KApplication::getApp()->getWindowSize().x, 20.0f);

		floor->addComponent(new KCSprite(floor, floorSize));
		floor->addComponent(new KCBoxCollider(floor, floorSize));
		floor->addComponent(new KCPhysicsBody(floor));
		floor->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->setMass(0.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			m_boxes.push_back(pCurrentScene->addEntityToScene());
			auto& box = m_boxes[i];
			box->setEntityTag(KTEXT("PhysicsBox"));
			box->addComponent(new KCSprite(box, Vec2f(10.0f, 10.0f)));
			box->addComponent(new KCBoxCollider(box, Vec2f(10.0f, 10.0f)));
			box->addComponent(new KCPhysicsBody(box));
			box->setIsInUse(false);
		}

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		auto floor = KApplication::getApp()->getCurrentScene()->findEntityByTag(KTEXT("floor"));
		floor->getComponent<KCTransform>()->setTranslation(Vec2f(0.0f, KApplication::getApp()->getWindowSize().y - 20.0f));
	}

	virtual void tick()
	{
		KComponentBase::tick();
		float dt = KApplication::getApp()->getDeltaTime();
		KCTransform* const transform = getEntity()->getComponent<KCTransform>();

		if (KInput::JustPressed(KKey::Escape))
		{
			KApplication::getApp()->closeApplication();
		}

		if (KInput::MouseJustPressed(KMouseButton::Left))
		{
			Vec2f mouseWorldPos = KInput::GetMouseWorldPosition();
			if (m_boxesAllocated + 1 != m_boxes.size())
			{
				m_boxes[m_boxesAllocated]->setIsInUse(true);
				m_boxes[m_boxesAllocated]->getComponent<KCTransform>()->setTranslation(mouseWorldPos);
				++m_boxesAllocated;
			}
		}
	}

private:

	std::vector<KEntity*> m_boxes;
	int32 m_boxesAllocated = 0;
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
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new CustomComponent(entity));
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

