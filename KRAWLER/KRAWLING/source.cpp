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
//#include "AnimationTest.h"

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




#include <JSON\json.hpp>
#include <fstream>

using json = nlohmann::json;
using namespace std;


int main(void)
{

	srand((unsigned)time(NULL));
	json j;
	ifstream file("res/maps/test_level.json");
	if (file.fail())
	{
		return -1;
		system("pause");
	}
	file >> j;
	
	_CrtDbgBreak();


	KApplicationInitialise initApp(true);
	StartupEngine(&initApp);
  
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, 2 * initApp.width, 2 * initApp.height)));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	//entity->addComponent(new PhysicsTest(entity));
	//entity->addComponent(new DeallocTest(entity));
	//entity->addComponent(new AnimationTest(entity));
	InitialiseSubmodules();

	//RunApplication();

	//ShutdownEngine();

	return 0;
}

