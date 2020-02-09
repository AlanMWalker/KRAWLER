#include "stdafx.h"
#include "Krawler.h"
#include "KApplication.h"

#include "KComponent.h"
#include "Components/KCBoxCollider.h"
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
		m_pSprite = new KCSprite(getEntity(), Vec2f(64, 64));
		getEntity()->addComponent(m_pSprite);
		return KInitStatus::Success;
	}

	virtual void onEnterScene()
	{
		sf::Texture* pTex = KAssetLoader::getAssetLoader().getTexture(KTEXT("pong_ball"));
		m_pSprite->setTexture(pTex);
		m_pSprite->setColour(Colour::White);
		getEntity()->getTransform()->setTranslation(50, 50);
	}

	virtual void tick() override
	{
	}

private:

	KCSprite* m_pSprite = nullptr;
};

void addComponents()
{
	pPaddleLeft->addComponent(new KCSprite(pPaddleLeft, PADDLE_SIZE));
	pPaddleLeft->addComponent(new PaddleControl(pPaddleLeft, KKey::W, KKey::S));
	pPaddleRight->addComponent(new KCSprite(pPaddleRight, PADDLE_SIZE));
	pPaddleRight->addComponent(new PaddleControl(pPaddleRight, KKey::Up, KKey::Down));
	pBall->addComponent(new Ball(pBall));
}

void allocateEntities(KApplication* pApp)
{
	KCHECK(pApp);

	auto pScene = pApp->getCurrentScene();

	{ // left paddle 
		pPaddleLeft = pScene->addEntityToScene();
		KCHECK(pPaddleLeft);
		pPaddleLeft->setTag(KTEXT("left_paddle"));
	}

	{ // right paddle
		pPaddleRight = pScene->addEntityToScene();
		KCHECK(pPaddleRight);
		pPaddleRight->setTag(KTEXT("right_paddle"));

	}

	{ // ball
		pBall = pScene->addEntityToScene();
		KCHECK(pBall);
		pBall->setTag(KTEXT("ball"));

	}
}


void setupGame()
{

}

int32 main(void)
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

	setupGame();

	RunApplication();
	ShutdownEngine();


	return static_cast<int32>(KInitStatus::Success);
}