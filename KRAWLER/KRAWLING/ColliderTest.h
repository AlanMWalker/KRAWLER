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
using namespace Krawler::Components;
using namespace Krawler::Input;


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
