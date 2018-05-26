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
#include <Components\KCAnimatedSprite.h>

//#include "OBBTest.h"
//#include "ColliderTest.h"
//#include "PhysicsTest.h"

#include <SFML\Graphics.hpp>
#include <KScene.h>

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;

#define BOX_SIZE 48
#define FLOOR_WIDTH 640

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
		auto pScene = KApplication::getApp()->getCurrentScene();
		KEntity* pEntity = pScene->addEntityToScene();
		pEntity->addComponent(new KCSprite(pEntity, Vec2f(20, 20)));
		pEntity->getTransformComponent()->setTranslation(50, 50);

		KAssetLoader& assetLoad = KAssetLoader::getAssetLoader();
		auto texture = assetLoad.getTexture(L"8ball");
		auto sound = assetLoad.getSound(L"fire");
		auto font = assetLoad.getFont(L"default_font");
		auto shader = assetLoad.getShader(L"default");
		pEntity->getComponent<KCSprite>()->setTexture(texture);

		bool result = pScene->addMultipleEntitiesToScene(9, entityVec);

		for (KEntity* pEntity : entityVec)
		{
			pEntity->addComponent(new KCSprite(pEntity, Vec2f(Maths::RandFloat(0, 100), Maths::RandFloat(0, 100))));
			pEntity->getTransformComponent()->setTranslation(Maths::RandFloat(0, 400), Maths::RandFloat(0, 400));
		}

		return KInitStatus::Success;
	}

	virtual void tick() override
	{
		if (KInput::JustPressed(KKey::Space))
		{
			for (KEntity* pEntity : entityVec)
			{
				KApplication::getApp()->getCurrentScene()->removeEntityFromScene(pEntity);
			}
			entityVec.clear();
		}
	}
private:
	std::vector<KEntity*> entityVec;
};

class AnimationTest : public KComponentBase
{
public:
	AnimationTest(KEntity* pEntity)
		: KComponentBase(pEntity)
	{

	}
	~AnimationTest() = default;
	KEntity* pEntity;
	virtual KInitStatus init() override
	{
		KApplication* pApp = KApplication::getApp();

		KScene* pScene = pApp->getCurrentScene();
		pEntity = pScene->addEntityToScene();
		KAssetLoader& assetLoader = KAssetLoader::getAssetLoader();

		pEntity->addComponent(new KCAnimatedSprite(pEntity, assetLoader.getAnimation(KTEXT("player_run_anim"))));
		pEntity->getComponent<KCAnimatedSprite>()->setRepeatingState(true);
		pEntity->getTransformComponent()->setTranslation(Vec2f(120, 210));
		pEntity->getTransformComponent()->setScale(10,10);
		return KComponentBase::init();
	}

	virtual void tick() override
	{
		if (KInput::Pressed(KKey::Space))
		{
			pEntity->getComponent<KCAnimatedSprite>()->play();
		}
		
		if (KInput::Pressed(KKey::P))
		{
			pEntity->getComponent<KCAnimatedSprite>()->pause();
		}

		if (KInput::Pressed(KKey::S))
		{
			pEntity->getComponent<KCAnimatedSprite>()->stop();
		}

		if (KInput::Pressed(KKey::Up))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("player_run_anim"));
		}
		if (KInput::Pressed(KKey::Down))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("enemy_run_anim"));
		}
		if (KInput::Pressed(KKey::Left))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("player_explode_anim"));
		}
		if (KInput::Pressed(KKey::Right))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("enemy_explode_anim"));
		}
	}

private:

};


int main(void)
{
	srand((unsigned)time(NULL));
	KApplicationInitialise initApp(true);
	StartupEngine(&initApp);

	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, 2 * initApp.width, 2 * initApp.height)));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	//entity->addComponent(new PhysicsTest(entity));
	//entity->addComponent(new DeallocTest(entity));
	entity->addComponent(new AnimationTest(entity));
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

