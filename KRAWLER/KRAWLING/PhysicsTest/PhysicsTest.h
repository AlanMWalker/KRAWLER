#pragma once
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

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;

#define FLOOR_WIDTH 100
#define BOX_SIZE 64

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
		for (int32 i = 0; i < CHUNK_POOL_SIZE - 3; ++i)
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

		if (KInput::MouseJustPressed(KMouseButton::Left))
		{
			Vec2f mouseWorldPos = KInput::GetMouseWorldPosition();
			if (m_boxesAllocated + 1 != m_boxes.size())
			{
				m_boxes[m_boxesAllocated]->setIsInUse(true);
				m_boxes[m_boxesAllocated]->getComponent<KCTransform>()->setTranslation(mouseWorldPos);
				//m_boxes[m_boxesAllocated]->getComponent<KCTransform>()->setRotation(Maths::RandFloat(0, 20));
				++m_boxesAllocated;
				KPrintf(KTEXT("Boxes allocated: %d\n"), m_boxesAllocated);
			}
		}
	}

private:

	std::vector<KEntity*> m_boxes;
	int32 m_boxesAllocated = 0;
};

