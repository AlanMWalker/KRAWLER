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
		const Vec2f BOX_BOUNDS(20.0f, 20.0f);
		const Vec2f FLOOR_BOUNDS(KCAST(float, GET_APP()->getWindowSize().x), 50);
		KScene* const pScene = GET_SCENE();


		for (int32 i = 0; i < BOX_COUNT; ++i)
		{
			auto const testBox = pScene->addEntityToScene();
			m_pBox = testBox;
			testBox->addComponent(new KCSprite(testBox, BOX_BOUNDS));
			auto& trans = *testBox->getTransform();

			const Vec2f RandPos(Maths::RandFloat(0, 250), Maths::RandFloat(0, 250));
			trans.setTranslation(RandPos);
			trans.setOrigin(BOX_BOUNDS * 0.5f);
			trans.setRotation(Maths::RandFloat(0, 359));
			//auto collider = new KCBoxCollider(testBox, Vec2f(BOX_BOUNDS));
			auto collider = new KCCircleCollider(testBox, BOX_BOUNDS.x / 2.0f);
			testBox->addComponent(collider);
			m_boxes.push_back(testBox);

			collider->subscribeCollisionCallback(&m_callback);
		}
		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		auto const boxTexture = ASSET().getTexture(L"8Ball");

		for (auto& box : m_boxes)
		{
			box->getComponent<KCSprite>()->setTexture(boxTexture);
		}
	}

	virtual void tick() override
	{
		static bool bOpen = true;
		auto imgui = getEntity()->getComponent<imguicomp>();
		imgui->update();
		imgui->begin("Box2D Testing");
		ImGui::Text("Position:");
		const std::string position = std::to_string(m_pBox->getTransform()->getPosition().x) + " " + std::to_string(m_pBox->getTransform()->getPosition().y);
		ImGui::Text(position.c_str());
		imgui->end();

		for (auto& b : m_boxes)
		{
			//b->getComponent<KCSprite>()->setColour(Colour::White);
		}

		m_boxes[0]->getTransform()->setTranslation(KInput::GetMouseWorldPosition());
	}

private:
	const int32 BOX_COUNT = 20;

	KEntity* m_pBox = nullptr;
	std::vector<KEntity*> m_boxes;

	KCColliderBaseCallback m_callback = [this](const KCollisionDetectionData& collData)
	{

		collData.entityA->getComponent<KCSprite>()->setColour(Colour::Green);
		collData.entityB->getComponent<KCSprite>()->setColour(Colour::Green);
		std::cout << "setting green" << std::endl;
	};

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
	initApp.windowTitle = KTEXT("Collision Module Check");
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

