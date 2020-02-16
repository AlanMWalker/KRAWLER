#include "stdafx.h"
#include "Krawler.h"
#include "KApplication.h"

#include "KComponent.h"
#include "Components/KCBoxCollider.h"
#include "Components/KCCircleCollider.h"
#include "Components/KCSprite.h"
#include "AssetLoader/KAssetLoader.h"

using namespace Krawler;
using namespace Components;
using namespace Input;

KEntity* pPaddleLeft = nullptr;
KEntity* pPaddleRight = nullptr;
KEntity* pBall = nullptr;

const Vec2f PADDLE_SIZE(32.0f, 96.0f);
const Vec2f BALL_SIZE(32, 32);

class PaddleControl : public KComponentBase
{
public:
	PaddleControl(KEntity* pEntity, KKey up, KKey down)
		: KComponentBase(pEntity), m_up(up), m_down(down), m_pTransform(nullptr)
	{
		setComponentTag(KTEXT("paddle"));
	}

	virtual KInitStatus init()
	{
		m_pTransform = getEntity()->getComponent<KCTransform>();
		KCHECK(m_pTransform);

		return KInitStatus::Success;
	}
	virtual void tick()
	{
		if (KInput::Pressed(m_up))
		{
			moveUp();
		}
		if (KInput::Pressed(m_down))
		{
			moveDown();
		}
	}
	void moveUp()
	{
		m_pTransform->move(0.0f, -MOVE_SPEED * KApplication::getApp()->getDeltaTime());
	}

	void moveDown()
	{
		m_pTransform->move(0.0f, MOVE_SPEED * KApplication::getApp()->getDeltaTime());
	}


private:
	KCTransform* m_pTransform;
	KKey m_up, m_down;
	const float MOVE_SPEED = 40.0f;
};

class Ball :
	public KComponentBase
{
public:
	Ball(KEntity* pEntity)
		: KComponentBase(pEntity)
	{
		setComponentTag(KTEXT("ball"));
	}

	virtual KInitStatus init() override
	{
		m_pSprite = new KCSprite(getEntity(), BALL_SIZE);
		getEntity()->addComponent(m_pSprite);

		KCCircleCollider* pCollider = new KCCircleCollider(getEntity(), BALL_SIZE.x / 2);
		if (!pCollider)
			return KInitStatus::Nullptr;
		getEntity()->addComponent(pCollider);

		pCollider->subscribeCollisionCallback(&m_collisionCallback);

		return KInitStatus::Success;
	}

	virtual void onEnterScene()
	{
		sf::Texture* pTex = KAssetLoader::getAssetLoader().getTexture(KTEXT("pong_ball"));
		m_pSprite->setTexture(pTex);
		m_pSprite->setColour(Colour::White);
		getEntity()->getTransform()->setOrigin(BALL_SIZE * 0.5f);
		getEntity()->getTransform()->setTranslation(KCAST(Vec2f, KApplication::getApp()->getWindowSize()) * 0.5f);

	}

	virtual void tick() override
	{
		const Vec2f mousePos = KInput::GetMouseWorldPosition();
		getEntity()->getTransform()->setTranslation(mousePos);

		if (m_bIsColliding)
		{
			m_bIsColliding = false;
		}
		else if (!m_bIsColliding)
		{
			m_pSprite->setColour(Colour::Red);
		}
	}

private:

	KCColliderBaseCallback m_collisionCallback = [this](const KCollisionDetectionData& collData)
	{
		if (!m_bIsColliding)
		{
			m_bIsColliding = true;
			m_pSprite->setColour(Colour::Green);
		}
	};

	KCSprite* m_pSprite = nullptr;
	bool m_bIsColliding = false;
};

void addComponents()
{
	{ // Left Paddle
		pPaddleLeft->addComponent(new KCSprite(pPaddleLeft, PADDLE_SIZE));
		pPaddleLeft->addComponent(new PaddleControl(pPaddleLeft, KKey::W, KKey::S));
		pPaddleLeft->addComponent(new KCBoxCollider(pPaddleLeft, PADDLE_SIZE));
	}

	{ // Right Paddle
		pPaddleRight->addComponent(new KCSprite(pPaddleRight, PADDLE_SIZE));
		pPaddleRight->addComponent(new PaddleControl(pPaddleRight, KKey::Up, KKey::Down));
		pPaddleRight->addComponent(new KCBoxCollider(pPaddleRight, PADDLE_SIZE));
	}

	{ // Ball
		pBall->addComponent(new Ball(pBall));

	}
}

void allocateEntities(KApplication* pApp)
{
	KCHECK(pApp);

	auto pScene = pApp->getCurrentScene();
	const auto SCREEN_SIZE = KCAST(Vec2f, KApplication::getApp()->getWindowSize());

	{ // left paddle 
		pPaddleLeft = pScene->addEntityToScene();
		KCHECK(pPaddleLeft);
		pPaddleLeft->setTag(KTEXT("left_paddle"));

		pPaddleLeft->getTransform()->setTranslation(Vec2f(
			5,
			(KCAST(float, SCREEN_SIZE.y) / 2.0f) - (PADDLE_SIZE.y * 0.5f)
		));
	}

	{ // right paddle
		pPaddleRight = pScene->addEntityToScene();
		KCHECK(pPaddleRight);
		pPaddleRight->setTag(KTEXT("right_paddle"));
		pPaddleRight->getTransform()->setTranslation(Vec2f(
			SCREEN_SIZE.x - 5 - PADDLE_SIZE.x,
			(KCAST(float, SCREEN_SIZE.y) / 2.0f) - (PADDLE_SIZE.y * 0.5f)
		));
	}

	{ // ball
		pBall = pScene->addEntityToScene();
		KCHECK(pBall);
		pBall->setTag(KTEXT("ball"));
	}
}

// -- Main -- 
#ifdef _DEBUG
int32 main(void)
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
#endif
{
	KApplicationInitialise init;
	init.width = 640;
	init.height = 480;
	init.windowTitle = KTEXT("Pong");
	init.gameFps = 60;
	init.physicsFps = 60;

	StartupEngine(&init);
	auto const pApp = KApplication::getApp();
	KCHECK(pApp);
	const std::wstring SceneName = KTEXT("PongScene");
	const Rectf SceneBounds(0.0f, 0.0f, (float)init.width, (float)init.height);
	KScene* const pScene = new KScene(SceneName, SceneBounds);
	pApp->getSceneDirector().addScene(pScene);
	pApp->getSceneDirector().setCurrentScene(SceneName);

	allocateEntities(pApp);
	addComponents();

	auto result = InitialiseSubmodules();
	if (result != KInitStatus::Success)
	{
		return static_cast<int32>(result);
	}

	RunApplication();
	ShutdownEngine();


	return static_cast<int32>(KInitStatus::Success);
}