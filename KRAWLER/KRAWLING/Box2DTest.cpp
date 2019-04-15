#include "stdafx.h"
#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Physics\KPhysicsScene.h>
#include <Utilities\KDebug.h>

#include <Box2D\Box2D.h>

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;


constexpr int32 VelocityIterations = 6;
constexpr int32 PositionIterations = 2;
const b2Vec2 gravity(0.0f, 9.81f);

b2World world(gravity);

class Box2DComp : public KComponentBase
{
public:

	Box2DComp(KEntity* pEntity) :
		KComponentBase(pEntity)
	{

	}

	virtual KInitStatus init() override
	{
		KScene* pScene = GET_SCENE();
		m_pBox = pScene->addEntityToScene();
		m_pBox->addComponent(new KCSprite(m_pBox, Vec2f(32, 32)));
		m_pBox->getTransformComponent()->setTranslation(Vec2f(32, 32));

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		m_pBox->getComponent<KCSprite>()->setColour(Colour::White);
	}

	virtual void tick() override
	{
		float dt = KApplication::getApp()->getDeltaTime();
		world.Step(dt, VelocityIterations, PositionIterations);
	}

private:
	KEntity* m_pBox;
};

#ifndef _CONSOLE
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
#else
int main(void)
#endif
{
	int32 i = sf::Texture::getMaximumSize();
	KApplicationInitialise initApp(false);
	initApp.gameFps = 60;
	initApp.physicsFps = 60;
	initApp.width = 1024;//sf::VideoMode::getDesktopMode().width;
	initApp.height = 768; // sf::VideoMode::getDesktopMode().height;
	initApp.windowStyle = KWindowStyle::Windowed_Fixed_Size;
	initApp.windowTitle = KTEXT("Testing TiledMaps");
	StartupEngine(&initApp);
	//const int sceneWidth = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->width;
	//const int sceneHeight = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->height;
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new Box2DComp(entity));
	//DO STUFF WITH ENTITY HERE
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

