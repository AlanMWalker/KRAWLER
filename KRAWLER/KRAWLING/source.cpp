#include "stdafx.h"
#include <vld.h>
#include <future>
#include <list>

#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Physics\KPhysicsScene.h>
#include <Utilities\KDebug.h>

#include <KEntity.h>
#include <Components\KCTransform.h>
#include <Components\KCSprite.h>
#include <Components\KCBoxCollider.h>
#include <Components\KCCircleCollider.h>
#include <Components\KCOrientedBoxCollider.h>
#include <Components\KCPhysicsBody.h>
#include <Components\KCAnimatedSprite.h>
#include <Components\KCTileMap.h>
//#include "OBBTest.h"
//#include "ColliderTest.h"
//#include "PhysicsTest.h"
#include "AnimationTest.h"

#include <SFML\Graphics.hpp>
#include <KScene.h>

#include <SFML\GpuPreference.hpp>

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;

#define BOX_SIZE 48
#define FLOOR_WIDTH 640

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

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


class TiledMapTest
	: public KComponentBase//KScene deallocation testing 
{
public:
	TiledMapTest(KEntity* pEntity) :
		KComponentBase(pEntity)
	{
	}
	~TiledMapTest() = default;

	virtual KInitStatus init() override
	{
		auto pScene = KApplication::getApp()->getCurrentScene();
		KAssetLoader& assetLoad = KAssetLoader::getAssetLoader();

		KEntity* pEntity = pScene->addEntityToScene();
		pEntity->addComponent(new KCTileMapSplit(pEntity, KTEXT("test_level")));
		//m_pShader = assetLoad.getShader(L"default");
		//m_pShader = assetLoad.getShader(L"ambient");
		//pEntity->getComponent<KCRenderableBase>()->setShader(m_pShader);
		//auto r = pEntity->getComponent<KCTileMapSplit>()->getTiledMapImportData()->properties.at(L"ambient_light_colour").type_colour;
		//m_pShader->setUniform("ambientColour", sf::Glsl::Vec4{ (float)(r.r) / 255.0f ,  (float)(r.g) / 255.0f ,(float)(r.b) / 255.0f ,(float)(r.a) / 255.0f });
		//m_pShader->setUniform("intensity", pEntity->getComponent<KCTileMapSplit>()->getTiledMapImportData()->properties.at(L"ambient_light_intensity").type_float);

		m_pPlayerEntity = pScene->addEntityToScene();
		m_pPlayerEntity->addComponent(new KCSprite(m_pPlayerEntity, Vec2f(16, 16)));
		m_pPlayerEntity->getComponent<KCSprite>()->setTexture(assetLoad.getTexture(L"dude"));
		KApplication::getApp()->getRenderer()->setSortType(Renderer::KRenderSortType::ZOrderSort);


		return KInitStatus::Success;
	}

	virtual void tick() override
	{
		const float dt = KApplication::getApp()->getDeltaTime();

		if (KInput::JustPressed(KKey::Escape))
		{
			KApplication::getApp()->closeApplication();
		}
		auto pTrans = m_pPlayerEntity->getTransformComponent();
		float movSpeed = 50.0f;
		if (KInput::Pressed(KKey::S))
		{
			pTrans->move(Vec2f(0.0f, movSpeed *dt));
		}
		if (KInput::Pressed(KKey::W))
		{
			pTrans->move(Vec2f(0.0f, -movSpeed *dt));
		}
		if (KInput::Pressed(KKey::A))
		{
			pTrans->move(Vec2f(-movSpeed *dt, 0.0f));
		}
		if (KInput::Pressed(KKey::D))
		{
			pTrans->move(Vec2f(movSpeed *dt, 0.0f));
		}
		Vec2i t(m_pPlayerEntity->getTransformComponent()->getTranslation());
		auto view = KApplication::getApp()->getRenderWindow()->getView();
		view.setCenter(Vec2f(t));
		KApplication::getApp()->getRenderWindow()->setView(view);
	}
private:
	std::vector<KEntity*> entityVec;
	KEntity* m_pPlayerEntity;
	sf::Shader* m_pShader;
	float intensity = 1.0f;
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
	initApp.width = sf::VideoMode::getDesktopMode().width;
	initApp.height = sf::VideoMode::getDesktopMode().height;
	initApp.windowStyle = KWindowStyle::Fullscreen;
	initApp.windowTitle = KTEXT("Testing TiledMaps");
	StartupEngine(&initApp);

	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, 2 * initApp.width, 2 * initApp.height)));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new TiledMapTest(entity));
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

