#include "stdafx.h"
#include "vld.h"
#include <future>
#include <list>

#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Physics\KPhysicsScene.h>
#include <TiledMap\KTiledMap.h>
#include <Utilities\KDebug.h>

#include <KEntity.h>
#include <Components\KCTransform.h>
#include <Components\KCSprite.h>
#include <Components\KCBoxCollider.h>
#include <Components\KCCircleCollider.h>
#include <Components\KCOrientedBoxCollider.h>
#include <Components\KCPhysicsBody.h>

#include <SFML\Graphics.hpp>
#include <KScene.h>

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;

#define BOX_SIZE 48
#define FLOOR_WIDTH 640

class PhysicsTest : public KComponentBase
{
public:
	PhysicsTest(KEntity* pEntity)
		: KComponentBase(pEntity)
	{

	}

	~PhysicsTest() = default;

	virtual KInitStatus init() override
	{
		auto pCurrentScene = KApplication::getApp()->getCurrentScene();
		for (int32 i = 0; i < MAX_NUMBER_OF_ENTITIES - 3; ++i)
		{
			m_boxes.push_back(pCurrentScene->addEntityToScene());
			auto& box = m_boxes[i];
			box->setEntityTag(KTEXT("PhysicsBox") + std::to_wstring(i + 1));

			box->addComponent(new KCSprite(box, Vec2f(BOX_SIZE, BOX_SIZE)));

			box->addComponent(new KCOrientedBoxCollider(box, Vec2f(BOX_SIZE, BOX_SIZE)));
			//box->addComponent(new KCCircleCollider(box, BOX_SIZE / 2));
			box->addComponent(new KCPhysicsBody(box));

			box->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->restitution = 0.8f;
			box->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->setMass(100.0f);
			box->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->staticFriction = 0.5;
			box->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->dynamicFriction = 0.2;
			box->setIsInUse(false);
		}
		auto floor = pCurrentScene->addEntityToScene();
		floor->setEntityTag(KTEXT("floor"));
		Vec2f floorSize(FLOOR_WIDTH, 20.0f);

		floor->addComponent(new KCSprite(floor, floorSize));
		floor->addComponent(new KCOrientedBoxCollider(floor, floorSize));
		floor->addComponent(new KCPhysicsBody(floor));
		floor->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->setMass(0.0f);
		floor->getComponent<KCPhysicsBody>()->getPhysicsBodyProperties()->restitution = (0.1f);



		auto pWorld = KApplication::getApp()->getPhysicsWorld();
		auto props = pWorld->getPhysicsWorldProperties();
		props.correctionPercentage = 0.8f;
		props.correctionThreshold = 0.01f;
		//props.gravity = Vec2f(0.0f, 0.0f);
		pWorld->setPhysicsWorldProperties(props);
		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		auto floor = KApplication::getApp()->getCurrentScene()->findEntityByTag(KTEXT("floor"));
		floor->getComponent<KCTransform>()->setTranslation(Vec2f(0.0f, KApplication::getApp()->getWindowSize().y - 20.0f));

		auto& asset = KAssetLoader::getAssetLoader();
		asset.setRootFolder(L"res\\");

		for (auto pBox : m_boxes)
		{
			pBox->getComponent<KCSprite>()->setColour(Colour(rand() % 256, rand() % 256, rand() % 256));
			//pBox->getComponent<KCSprite>()->setTexture(asset.loadTexture(L"8ball.png"));
			//pBox->getComponent<KCTransform>()->setOrigin(BOX_SIZE / 2, BOX_SIZE / 2);
		}
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

		auto floor = KApplication::getApp()->getCurrentScene()->findEntityByTag(KTEXT("floor"));
		floor->getComponent<KCTransform>()->setTranslation(Vec2f(0.0f, KApplication::getApp()->getWindowSize().y - 20.0f));

		if (KInput::MousePressed(KMouseButton::Left) || KInput::MouseJustPressed(KMouseButton::Right))
		{
			Vec2f mouseWorldPos = KInput::GetMouseWorldPosition();
			if (m_boxesAllocated + 1 != m_boxes.size())
			{
				m_boxes[m_boxesAllocated]->setIsInUse(true);
				m_boxes[m_boxesAllocated]->getComponent<KCTransform>()->setTranslation(mouseWorldPos);
				m_boxes[m_boxesAllocated]->getComponent<KCTransform>()->setRotation(Maths::RandFloat(0, 20));
				++m_boxesAllocated;
				KPrintf(KTEXT("Boxes allocated: %d\n"), m_boxesAllocated);
			}
		}
	}

private:

	std::vector<KEntity*> m_boxes;
	int32 m_boxesAllocated = 0;
};

class ColliderTest : public KComponentBase
{
public:

	ColliderTest(KEntity* pEntity)
		: KComponentBase(pEntity)
	{
		pEntity->setEntityTag(KTEXT("level setup"));
	}

	~ColliderTest() = default;

	virtual KInitStatus init() override
	{
		auto pCurrentScene = KApplication::getApp()->getCurrentScene();

		m_pBoxA = pCurrentScene->addEntityToScene();
		m_pBoxB = pCurrentScene->addEntityToScene();

		//box a init
		if (!m_pBoxA->addComponent(new KCSprite(m_pBoxA, Vec2f(BOX_SIZE, BOX_SIZE))))
		{
			return KInitStatus::Failure;
		}

		//if (!m_pBoxA->addComponent(new KCCircleCollider(m_pBoxA, BOX_SIZE / 2.0f)))
		if (!m_pBoxA->addComponent(new KCCircleCollider(m_pBoxA, BOX_SIZE / 2.0f)))
		{
			return KInitStatus::Failure;
		}
		m_pBoxA->addComponent(new KCPhysicsBody(m_pBoxA));
		m_pBoxB->addComponent(new KCPhysicsBody(m_pBoxB));
		//box b init
		if (!m_pBoxB->addComponent(new KCSprite(m_pBoxB, Vec2f(BOX_SIZE, BOX_SIZE))))
		{
			return KInitStatus::Failure;
		}

		if (!m_pBoxB->addComponent(new KCCircleCollider(m_pBoxB, BOX_SIZE / 2.0f)))
		{
			return KInitStatus::Failure;
		}

		m_pBoxA->setEntityTag(KTEXT("box a"));
		m_pBoxB->setEntityTag(KTEXT("box b"));
		m_pBoxB->getComponent<KCTransform>()->setOrigin(Vec2f(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f));
		m_pBoxA->getComponent<KCTransform>()->setOrigin(Vec2f(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f));

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		m_pBoxA->getComponent<KCTransform>()->setTranslation(Vec2f(100, 100));

		m_pBoxB->getComponent<KCCircleCollider>()->subscribeCollisionCallback(&m_callback);

		KAssetLoader& rAssetLoader = KAssetLoader::getAssetLoader();
		rAssetLoader.setRootFolder(KTEXT("res\\"));
		auto pTexture = rAssetLoader.loadTexture(KTEXT("8ball.png"));
		//KCHECK(pTexture);
		m_pBoxA->getComponent<KCSprite>()->setTexture(pTexture);
		m_pBoxB->getComponent<KCSprite>()->setTexture(pTexture);

	}

	virtual void tick() override
	{
		if (!m_bHasCollided)
		{
			m_pBoxB->getComponent<KCSprite>()->setColour(Colour::Red);
		}
		m_pBoxB->getComponent<KCTransform>()->setTranslation(KInput::GetMouseWorldPosition());

		if (KInput::MouseJustPressed(KMouseButton::Left))
		{
			Vec2f world = KInput::GetMouseWorldPosition();
			KPrintf(KTEXT("Mouse: %f : %f\n"), world.x, world.y);
		}

		if (KInput::JustPressed(KKey::Escape))
			KApplication::getApp()->closeApplication();
		Vec2f pos = m_pBoxB->getComponent<KCCircleCollider>()->getCentrePosition();
		//pos += m_pBoxB->getComponent<KCBoxCollider>()->getHalfSize();
		KPrintf(KTEXT("Mouse: %f : %f\n"), pos.x, pos.y);

		m_bHasCollided = false;
	}

private:
	void setCollided()
	{
		m_bHasCollided = true;
		m_pBoxB->getComponent<KCSprite>()->setColour(Colour::Green);
	}

	KCColliderBaseCallback m_callback = [this](const KCollisionDetectionData&) -> void
	{
		setCollided();
	};

	bool m_bHasCollided = false;
	KEntity* m_pBoxA;
	KEntity* m_pBoxB;
};

class OBBTest :
	public KComponentBase
{
public:
	OBBTest(KEntity* pEntity)
		: KComponentBase(pEntity)
	{
		pEntity->setEntityTag(KTEXT("level setup"));
	}

	~OBBTest() = default;

	virtual KInitStatus init() override
	{
		auto pCurrentScene = KApplication::getApp()->getCurrentScene();

		m_pBoxA = pCurrentScene->addEntityToScene();
		m_pBoxB = pCurrentScene->addEntityToScene();

		//box a init
		if (!m_pBoxA->addComponent(new KCSprite(m_pBoxA, Vec2f(BOX_SIZE, BOX_SIZE))))
		{
			return KInitStatus::Failure;
		}

		if (!m_pBoxA->addComponent(new KCCircleCollider(m_pBoxA, BOX_SIZE / 2.0f)))
		{
			return KInitStatus::Failure;
		}

		//box b init
		if (!m_pBoxB->addComponent(new KCSprite(m_pBoxB, Vec2f(BOX_SIZE, BOX_SIZE))))
		{
			return KInitStatus::Failure;
		}

		if (!m_pBoxB->addComponent(new KCOrientedBoxCollider(m_pBoxB, Vec2f(BOX_SIZE, BOX_SIZE))))
		{
			return KInitStatus::Failure;
		}

		m_pBoxA->setEntityTag(KTEXT("box a"));
		m_pBoxB->setEntityTag(KTEXT("box b"));
		m_pBoxB->getComponent<KCTransform>()->setOrigin(Vec2f(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f));
		m_pBoxA->getComponent<KCTransform>()->setOrigin(Vec2f(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f));

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		m_pBoxA->getComponent<KCTransform>()->setTranslation(Vec2f(100, 100));

		m_pBoxB->getComponent<KCColliderBase>()->subscribeCollisionCallback(&m_callback);

		KAssetLoader& rAssetLoader = KAssetLoader::getAssetLoader();
		rAssetLoader.setRootFolder(KTEXT("res\\"));
		auto pTexture = rAssetLoader.loadTexture(KTEXT("8ball.png"));
		//KCHECK(pTexture);
		m_pBoxA->getComponent<KCSprite>()->setTexture(pTexture);
		//m_pBoxB->getComponent<KCSprite>()->setTexture(pTexture);
	}

	virtual void tick() override
	{
		if (!m_bHasCollided)
		{
			m_pBoxB->getComponent<KCSprite>()->setColour(Colour::Red);
		}
		m_pBoxB->getComponent<KCTransform>()->setTranslation(KInput::GetMouseWorldPosition());

		if (KInput::MouseJustPressed(KMouseButton::Left))
		{
			Vec2f world = KInput::GetMouseWorldPosition();
			KPrintf(KTEXT("Mouse: %f : %f\n"), world.x, world.y);
		}

		if (KInput::JustPressed(KKey::Q))
			m_pBoxB->getComponent<KCTransform>()->rotate(-10);
		if (KInput::JustPressed(KKey::E))
			m_pBoxB->getComponent<KCTransform>()->rotate(10);

		if (KInput::JustPressed(KKey::O))
			m_pBoxA->getComponent<KCTransform>()->rotate(-10);
		if (KInput::JustPressed(KKey::P))
			m_pBoxA->getComponent<KCTransform>()->rotate(10);

		if (KInput::JustPressed(KKey::Left))
			m_pBoxA->getComponent<KCTransform>()->move(-10, 0);
		if (KInput::JustPressed(KKey::Right))
			m_pBoxA->getComponent<KCTransform>()->move(10, 0);

		if (KInput::JustPressed(KKey::Escape))
			KApplication::getApp()->closeApplication();
		//Vec2f pos = m_pBoxB->getComponent<KCCircleCollider>()->getCentrePosition();
		//pos += m_pBoxB->getComponent<KCBoxCollider>()->getHalfSize();
		//KPrintf(KTEXT("Mouse: %f : %f\n"), pos.x, pos.y);

		m_bHasCollided = false;
	}

private:
	void setCollided()
	{
		m_bHasCollided = true;
		m_pBoxB->getComponent<KCSprite>()->setColour(Colour::Green);
	}

	KCColliderBaseCallback m_callback = [this](const KCollisionDetectionData& data) -> void
	{
		setCollided();
	};

	bool m_bHasCollided = false;
	KEntity* m_pBoxA;
	KEntity* m_pBoxB;

};

class DeallocTest
	: public KComponentBase//KScene deallocation testing 
{
public:
	DeallocTest(KEntity* pEntity) :
		KComponentBase(pEntity)
	{
	}
	~DeallocTest() = default;

	virtual KInitStatus init() override
	{

		KEntity* pEntity = KApplication::getApp()->getCurrentScene()->addEntityToScene();
		pEntity->addComponent(new KCSprite(pEntity, Vec2f(20, 20)));
		pEntity->getTransformComponent()->setTranslation(50, 50);

		return KInitStatus::Success;
	}

	virtual void tick() override
	{

	}
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
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, 2 * initApp.width, 2 * initApp.height)));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	//entity->addComponent(new PhysicsTest(entity));
	entity->addComponent(new DeallocTest(entity));
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

