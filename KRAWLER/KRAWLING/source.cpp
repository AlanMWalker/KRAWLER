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
#include <Tiled\KTiledImport.h>
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
	_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
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

		pEntity = pScene->addEntityToScene();
		pEntity->addComponent(new KCTileMapSplit(pEntity, KTEXT("test_level")));
		//m_pShader = assetLoad.getShader(L"default");
		m_pShader = assetLoad.getShader(L"ambient");
		pEntity->getComponent<KCRenderableBase>()->setShader(m_pShader);
		auto pImportData = pEntity->getComponent<KCTileMapSplit>()->getTiledMapImportData();
		auto r = pImportData->properties.at(L"ambient_light_colour").type_colour;
		m_pShader->setUniform("ambientColour", sf::Glsl::Vec4{ (float)(r.r) / 255.0f ,  (float)(r.g) / 255.0f ,(float)(r.b) / 255.0f ,(float)(r.a) / 255.0f });
		m_pShader->setUniform("intensity", pImportData->properties.at(L"ambient_light_intensity").type_float);

		m_pPlayerEntity = pScene->addEntityToScene();
		m_pPlayerEntity->addComponent(new KCSprite(m_pPlayerEntity, Vec2f(32, 32)));

		KPhysicsBodyProperties properties;
		if (pImportData->properties.count(KTEXT("player_mass")) > 0)
		{
			properties.setMass(pImportData->properties.at(KTEXT("player_mass")).type_float);
		}
		properties.restitution = 0.10f;

		m_pPlayerEntity->addComponent(new KCPhysicsBody(m_pPlayerEntity, properties));
		m_pPlayerEntity->addComponent(new KCBoxCollider(m_pPlayerEntity, Vec2f(32.0f, 36.0f)));
		m_pPlayerEntity->getTransformComponent()->setOrigin(Vec2f(16.0f, 16.0f));
		m_pPlayerEntity->getComponent<KCSprite>()->setTexture(assetLoad.getTexture(L"dude"));
		KApplication::getApp()->getRenderer()->setSortType(Renderer::KRenderSortType::ZOrderSort);
		Physics::KPhysicsWorldProperties props;
		props.gravity = Vec2f(0.0f, 0.0f);
		KApplication::getApp()->getPhysicsWorld()->setPhysicsWorldProperties(props);

		if (pImportData->properties.count(KTEXT("player_walk_force")))
		{
			m_player_walk_force = pImportData->properties.at(KTEXT("player_walk_force")).type_float;
		}

		if (pImportData->properties.count(KTEXT("player_friction_timer")))
		{
			m_player_friction_time = pImportData->properties.at(KTEXT("player_friction_timer")).type_float;
		}

		if (pImportData->properties.count(KTEXT("max_player_velocity")))
		{
			m_max_player_velocity = pImportData->properties.at(KTEXT("max_player_velocity")).type_float;
		}

		if (pImportData->properties.count(KTEXT("player_friction_multiplier")))
		{
			m_friction_multiplier = pImportData->properties.at(KTEXT("player_friction_multiplier")).type_float;
		}

		if (pImportData->properties.count(KTEXT("player_walk_force_initial")))
		{
			m_initial_walk_force = pImportData->properties.at(KTEXT("player_walk_force_initial")).type_float;
		}

		return KInitStatus::Success;
	}
	virtual void onEnterScene()
	{
		KCTileMapSplit* pSplit = pEntity->getComponent<KCTileMapSplit>();
		for (auto& line : pSplit->getHorizontalTileLines())
			line.setShader(m_pShader);
	}
	virtual void tick() override
	{
		const float dt = KApplication::getApp()->getDeltaTime();

		if (KInput::JustPressed(KKey::Escape))
		{
			KApplication::getApp()->closeApplication();
		}

		auto pPhysBody = m_pPlayerEntity->getComponent<KCPhysicsBody>();
		static constexpr float MAX_MOVE_SPEED = 50.0f;
		static float pressTimer = 0.0f;

		m_force = Vec2f(0.0f, 0.0f);

		if (KInput::Pressed(KKey::S))
		{
			m_force.y = 1.0f;
		}

		if (KInput::Pressed(KKey::W))
		{
			m_force.y = -1.0f;
		}

		if (KInput::Pressed(KKey::A))
		{
			m_force.x = -1.0f;
		}

		if (KInput::Pressed(KKey::D))
		{
			m_force.x = 1.0f;
		}

		if (m_force != Vec2f(0.0f, 0.0f))
		{
			if (pressTimer < 1.0f)
			{
				pressTimer += dt;
			}
			else
			{
				pressTimer = 1.0f;
			}
		}
		else
		{
			pressTimer = 0.0f;
		}

		if (KInput::JustPressed(KKey::F2))
		{
			KPrintf(L"Player Velocity: %f %f or ( %f in total)\n", pPhysBody->getVelocity().x, pPhysBody->getVelocity().y, GetLength(pPhysBody->getVelocity()));
		}

		if (GetSquareLength(pPhysBody->getVelocity()) < 2.0f&& m_force != Vec2f(0.0f, 0.0f))
		{
			pPhysBody->applyForce(Normalise(m_force) * ((1.0f - pressTimer)  * m_initial_walk_force));
			//KPrintf(L"Applying initial force! %f (Press timer = %f) \n", (1.0f - pressTimer) *m_initial_walk_force * m_force.x, pressTimer);
		}
		else
		{
			pPhysBody->applyForce(Normalise(m_force) * m_player_walk_force);
		}



		//if not moving apply frictional force



		Vec2i t(m_pPlayerEntity->getTransformComponent()->getTranslation());
		auto view = KApplication::getApp()->getRenderWindow()->getView();
		view.setCenter(Vec2f(t));
		KApplication::getApp()->getRenderWindow()->setView(view);
	}

	virtual void fixedTick() override
	{
		auto pPhysBody = m_pPlayerEntity->getComponent<KCPhysicsBody>();
		const float sqLengthVel = GetSquareLength(pPhysBody->getVelocity());
		if (sqLengthVel > (m_max_player_velocity* m_max_player_velocity))
		{
			pPhysBody->setVelocity(Normalise(pPhysBody->getVelocity()) * m_max_player_velocity);
		}

		if (m_force == Vec2f(0.0f, 0.0f) && sqLengthVel < 9.0f)
		{
			pPhysBody->setVelocity(Vec2f(0.0f, 0.0f));
		}

		pPhysBody->applyForce(Normalise(-pPhysBody->getVelocity()) * (m_player_walk_force * m_friction_multiplier));
	}

private:
	std::vector<KEntity*> entityVec;
	KEntity* m_pPlayerEntity;
	KEntity* pEntity;
	sf::Shader* m_pShader;
	Vec2f m_force;
	float intensity = 1.0f;
	float m_player_walk_force = 0.0f;
	float m_player_friction_time = 0.0f;
	float m_max_player_velocity = 0.0f;
	float m_friction_multiplier = 0.8f;
	float m_initial_walk_force = 0.0f;
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
	const int sceneWidth = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->width;
	const int sceneHeight = KAssetLoader::getAssetLoader().getLevelMap(L"test_level")->height;
	auto app = KApplication::getApp();
	app->getSceneDirector().addScene(new KScene(std::wstring(KTEXT("SceneA")), Rectf(0, 0, (70 * 32), (40 * 32))));
	app->getSceneDirector().setCurrentScene(KTEXT("SceneA"));
	auto pCurrentScene = app->getCurrentScene();
	auto entity = pCurrentScene->addEntityToScene();
	entity->addComponent(new TiledMapTest(entity));
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

