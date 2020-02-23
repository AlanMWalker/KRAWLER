#include "stdafx.h"

// stl
#include <array>

// engine
#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Utilities\KDebug.h>

//external
#include "imgui-SFML.h"
#include "imgui.h"

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;
using namespace Krawler::Maths;

void processEvent(const sf::Event& e)
{
	ImGui::SFML::ProcessEvent(e);
}

// custom imgui component
class imguicomp : public KComponentBase
{
public:
	imguicomp(KEntity* pEntity) : KComponentBase(pEntity) {}
	~imguicomp() = default;

	virtual KInitStatus init() override
	{
		ImGui::SFML::Init(*KApplication::getApp()->getRenderWindow());
		std::function<void(void)> subLastDraw = std::bind(&imguicomp::draw, this);
		KApplication::getApp()->subscribeToEventQueue(processEvent);
		KApplication::getApp()->getRenderer()->subscribeLastDrawCallback(subLastDraw);
		return KInitStatus::Success;
	}

	virtual void cleanUp() override
	{
		ImGui::SFML::Shutdown();
	}


private:
	void draw()
	{
		ImGui::SFML::Render(*KApplication::getApp()->getRenderWindow());
	}

};


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

		

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
	}

	virtual void tick() override
	{
		ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
		ImGui::Begin("Physics Setup Editor");
		

		ImGui::End();
	}

private:
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
	initApp.windowTitle = KTEXT("New Physics Component Check");
	StartupEngine(&initApp);
	//const int sceneWidth = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->width;
	//const int sceneHeight = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->height;
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new Box2DComp(entity));
	entity->addComponent(new imguicomp(entity));
	//DO STUFF WITH ENTITY HERE
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

