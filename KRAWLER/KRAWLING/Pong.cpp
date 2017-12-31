#include "stdafx.h"
#include "Krawler.h"
#include "KApplication.h"

#include "KComponent.h"

#include "Components\KCBoxCollider.h"
#include "Components\KCSprite.h"

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
		m_pTransform->move(0.0f, -MOVE_SPEED*KApplication::getApp()->getDeltaTime());
	}

	void moveDown()
	{
		m_pTransform->move(0.0f, MOVE_SPEED*KApplication::getApp()->getDeltaTime());
	}


private:
	KCTransform* m_pTransform;
	KKey m_up, m_down;
	const float MOVE_SPEED = 40.0f;
};



void addComponents()
{
	pPaddleLeft->addComponent(new KCSprite(pPaddleLeft, PADDLE_SIZE));
	pPaddleLeft->addComponent(new PaddleControl(pPaddleLeft, KKey::W, KKey::S));
	pPaddleRight->addComponent(new KCSprite(pPaddleRight, PADDLE_SIZE));
	pPaddleRight->addComponent(new PaddleControl(pPaddleRight, KKey::Up, KKey::Down));
	pBall->addComponent(new KCSprite(pBall, BALL_SIZE));
}

void setupGame()
{

}

int main(void)
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

	pPaddleLeft = pScene->addEntityToScene();
	KCHECK(pPaddleLeft);
	pPaddleRight = pScene->addEntityToScene();
	KCHECK(pPaddleRight);
	pBall = pScene->addEntityToScene();
	KCHECK(pBall);

	addComponents();

	KINIT_CHECK(InitialiseSubmodules());

	setupGame();

	RunApplication();
	ShutdownEngine();


	return Success;
}