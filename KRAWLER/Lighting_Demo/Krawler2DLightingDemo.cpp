#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader/KAssetLoader.h>
#include <KComponent.h>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <functional>
#ifndef _CONSOLE
#include <Windows.h>
#endif

using namespace Krawler;
using namespace Krawler::Components;

// callback for processing events (only required for custom event processing that must occur during SFML event processing, such as IMGUI)
void processEvent(const sf::Event& e)
{
	ImGui::SFML::ProcessEvent(e);
}

class LightPoly : public KCRenderableBase
{
public:

	LightPoly(KEntity* pEntity) :
		KCRenderableBase(pEntity)
	{

	}
	~LightPoly() = default;

	virtual KInitStatus init() override
	{
		auto& loader = KAssetLoader::getAssetLoader();
		setShader(loader.getShader(KTEXT("light_draw")));

		// @Checkup
		// need to pre-compute unique points from all world colliders
		// but no way to query world colliders easily atm
		// also, all blocked tiles are entities that exist rather than being a bunch of geometry we check against
		

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{

	}

	virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const override
	{
		rStates.blendMode = sf::BlendAlpha;
		rTarget.draw(m_vertices, rStates);
	}

	virtual void cleanUp() override
	{
		m_vertices.clear();
	}

	virtual Krawler::Rectf getOnscreenBounds() const { return Rectf(0, 0, 0, 0); };


private:

	void createStaticLightPoly();

	sf::VertexArray m_vertices;
	std::vector<Vec2f> m_uniquePoints;
};


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

	virtual void tick() override
	{
		static float temp;
		ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
		ImGui::Begin("Light Data");
		ImGui::InputFloat("Intensity", &temp);
		ImGui::End();
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

#ifndef _CONSOLE
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

	// imgui stuff
	{
		auto imguiEntity = pCurrentScene->addEntityToScene();
		imguiEntity->addComponent(new imguicomp(imguiEntity));
		imguiEntity->addComponent(new KCTileMapSplit(imguiEntity, KTEXT("test_level")));
	}

	// lights stuff
	{
		auto& assets = KAssetLoader::getAssetLoader();
		auto pLevelMap = assets.getLevelMap(KTEXT("test_level"));
		int32 lightLayerIndex = -1;
		for (int32 i = 0; i < pLevelMap->layersVector.size(); ++i)
		{
			if (pLevelMap->layersVector[i].name == KTEXT("lights"))
			{
				lightLayerIndex = i;
				break;
			}
		}

		auto& lightsVector = pLevelMap->layersVector[lightLayerIndex].objectsVector;
		for (auto& light : lightsVector)
		{
			auto pLightEntity = pCurrentScene->addEntityToScene();
			KCHECK(pLightEntity);
			pLightEntity->addComponent(new LightPoly(pLightEntity));
			pLightEntity->getTransformComponent()->setTranslation(Vec2f(light.x, light.y));
		}

	}

	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

void LightPoly::createStaticLightPoly()
{
	auto pCurrentScene = KApplication::getApp()->getCurrentScene();
	KCollisionOverseer overseer = pCurrentScene->getCollisionOverseer();

}
