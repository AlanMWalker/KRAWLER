#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader/KAssetLoader.h>
#include <KComponent.h>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <functional>

using namespace Krawler;
using namespace Krawler::Components;

void processEvent(const sf::Event& e)
{
	ImGui::SFML::ProcessEvent(e);
}

class TempComponent : public KComponentBase
{
public:
	TempComponent(KEntity* pEntity) : KComponentBase(pEntity) {}
	~TempComponent() = default;

	virtual KInitStatus init() override
	{
		ImGui::SFML::Init(*KApplication::getApp()->getRenderWindow());
		std::function<void(void)> subLastDraw = std::bind(&TempComponent::draw, this);
		//std::function<void(const sf::Event&)> subLastDraw = std::bind(&TempComponent::processEvent, this);
		KApplication::getApp()->subscribeToEventQueue(processEvent);
		KApplication::getApp()->getRenderer()->subscribeLastDrawCallback(subLastDraw);
		return KInitStatus::Success;
	}

	virtual void tick() override
	{
		static float temp;
		ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
		ImGui::Begin("fuck off");
		ImGui::InputFloat("Jesus",&temp);
		ImGui::End();
	}


private:


	void draw()
	{
		ImGui::SFML::Render(*KApplication::getApp()->getRenderWindow());
	}

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
	initApp.windowTitle = KTEXT("Krawler Lighting Demo");
	StartupEngine(&initApp);
	const int sceneWidth = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->width;
	const int sceneHeight = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->height;
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new TempComponent(entity));

	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

