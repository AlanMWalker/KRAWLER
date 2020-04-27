#include "stdafx.h"

// stl
#include <array>

// engine
#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Utilities\KDebug.h>

#include <Components\KCBoxCollider.h>
#include <Components\KCCircleCollider.h>
#include <Components\KCBody.h>
#include <Components\KCTileMap.h>

//external
#include "imgui-SFML.h"
#include "imgui.h"

#ifdef _DEBUG
// CRT 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

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
		m_bWasInitSuccessful = true;
		return KInitStatus::Success;
	}

	virtual void cleanUp() override
	{
		ImGui::SFML::Shutdown();
	}

	// call to invoke imgui::begin
	void begin(const std::string& name)
	{
		if (!m_bBeginCalled)
		{
			ImGui::Begin(name.c_str());
			m_bBeginCalled = true;
		}
	}

	// call to invoke imgui::end
	void end()
	{
		if (!m_bEndCalled)
		{
			m_bEndCalled = true;
			ImGui::End();
		}
	}

	// Call to invoke imgui::sfml::update
	void update()
	{
		if (!m_bUpdateRun)
		{
			ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
			m_bUpdateRun = true;
		}
	}

private:

	void draw()
	{
		// If init is unsuccessful we'll avoid invoking render since this can force an abrupt halt 
		// instead we'll print that we're skipping an imgui render call 
		if (m_bWasInitSuccessful)
		{
			static bool hasBeenPrint = false;
			if (!hasBeenPrint)
			{
				KPRINTF("Skipping imgui draw");
				hasBeenPrint = true;
			}
		}

		// If the update hasn't been run yet, we should totally avoid calling imgui draw
		// this is an applicating halting mistake to make.
		if (!m_bUpdateRun)
		{
			return;
		}


		// If begin was not called, do not try render. 
		// This is an application halting mistake to make.
		if (!m_bBeginCalled)
		{
			return;
		}

		// If end was not called, do not try render. 
		// This is an application halting mistake to make.
		if (!m_bEndCalled)
		{
			return;
		}

		ImGui::SFML::Render(*KApplication::getApp()->getRenderWindow());
		m_bUpdateRun = false;
		m_bBeginCalled = false;
		m_bEndCalled = false;
	}

	bool m_bUpdateRun = false;
	bool m_bWasInitSuccessful = false;
	bool m_bBeginCalled = false;
	bool m_bEndCalled = false;
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
		auto zsortMap = ASSET().getLevelMap(L"zsort");

		if(!zsortMap)
			KPRINTF("Failed to load zsort\n");
		else
			KPRINTF("Loaded zsort\n");

		getEntity()->addComponent(new KCTileMap(getEntity(), L"test_level"));

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
	
	}

	virtual void tick() override
	{
		static bool bOpen = true;
		auto imgui = getEntity()->getComponent<imguicomp>();
		imgui->update();
		imgui->begin("Box2D Testing");
		imgui->end();

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
	initApp.windowTitle = KTEXT("Tile Map Rendering Test");
	StartupEngine(&initApp);
	//const int sceneWidth = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->width;
	//const int sceneHeight = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->height;
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("Main_Scene")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("Test_Scene")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().setStartScene(KTEXT("Main_Scene"));
	auto entity = GET_SCENE()->addEntityToScene();
	entity->addComponent(new Box2DComp(entity));
	entity->addComponent(new imguicomp(entity));
	

	//DO STUFF WITH ENTITY HERE
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();
	_CrtDumpMemoryLeaks();
	return 0;
}

