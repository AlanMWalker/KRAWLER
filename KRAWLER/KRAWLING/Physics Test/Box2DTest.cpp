#include "stdafx.h"

// stl
#include <array>

// engine
#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Utilities\KDebug.h>

#include <Components\KCBody.h>
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
		const Vec2f BOX_BOUNDS(20, 20);
		const Vec2f FLOOR_BOUNDS(KCAST(float, GET_APP()->getWindowSize().x), 50);
		KScene* pScene = GET_SCENE();

		for (int32 i = 0; i < BOX_COUNT; ++i)
		{
			auto testBox = pScene->addEntityToScene();
			m_pBox = testBox;
			testBox->addComponent(new KCSprite(testBox, BOX_BOUNDS));
			auto& trans = *testBox->getTransform();

			trans.setOrigin(BOX_BOUNDS * 0.5f);
			const Vec2f RandPos(Maths::RandFloat(0, 700), Maths::RandFloat(0, 150));
			trans.setTranslation(RandPos);

			KMatDef matDef;
			matDef.density = 1.0f;
			KBodyDef bodyDef;
			bodyDef.bodyType = BodyType::Dynamic_Body;
			bodyDef.position = RandPos;
			bodyDef.bActive = true;

			//testBox->setIsInUse(false);
			testBox->addComponent(new KCBody(*testBox, BOX_BOUNDS, bodyDef, matDef));
			m_boxes.push_back(testBox);
		}

		{// static box

			auto floor = pScene->addEntityToScene();
			floor->addComponent(new KCSprite(floor, FLOOR_BOUNDS));
			auto& trans = *floor->getTransform();

			const Vec2f FloorPos = Vec2f(FLOOR_BOUNDS.x * 0.5f, GET_APP()->getWindowSize().y - FLOOR_BOUNDS.y * 0.5f);
			trans.setOrigin(FLOOR_BOUNDS * 0.5f);
			trans.setTranslation(FloorPos);

			KBodyDef bodyDef;
			bodyDef.bodyType = BodyType::Static_Body;
			bodyDef.position = Vec2f(FloorPos);

			floor->addComponent(new KCBody(*floor, FLOOR_BOUNDS, bodyDef));
		}

		GET_APP()->getPhysicsWorld().setGravity(Vec2f());
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
		//ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
		//ImGui::Begin("Physics Setup Editor", &bOpen);
		imgui->begin("Box2D Testing");
		ImGui::Text("Position:");
		std::string position = std::to_string(m_pBox->getTransform()->getPosition().x) + " " + std::to_string(m_pBox->getTransform()->getPosition().y);
		ImGui::Text(position.c_str());
		//ImGui::End();
		imgui->end();
		if (KInput::JustPressed(KKey::D))
		{
			for (auto box : m_boxes)
			{
				box->getComponent<KCBody>()->applyLinearImpulseToCenter(Vec2f(100000000, 0), true);
			}
		}
		changeScene();

	}

	void changeScene()
	{
		if (KInput::JustPressed(KKey::C))
		{
			GET_APP()->getSceneDirector().transitionToScene(KTEXT("Test_Scene"));
		}
	}

private:
	const int32 BOX_COUNT = 16;

	KEntity* m_pBox = nullptr;
	std::vector<KEntity*> m_boxes;

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

	return 0;
}

