#include "stdafx.h"

// stl
#include <array>

// engine
#include <Krawler.h>
#include <KApplication.h>
#include <AssetLoader\KAssetLoader.h>
#include <Utilities\KDebug.h>
#include <Box2D\Box2D.h>

//external
#include "imgui-SFML.h"
#include "imgui.h"

using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;
using namespace Krawler::Maths;

constexpr int32 VelocityIterations = 6;
constexpr int32 PositionIterations = 2;

const Vec2f FloorBounds{ 400, 30 };

const b2Vec2 gravity(0.0f, 9.81f);

b2World world(b2Vec2(0, 0));

Vec2f b2Vec2toVec2f(const b2Vec2& vec)
{
	return Vec2f(vec.x, vec.y);
}

b2Vec2 Vec2ftob2Vec2(const Vec2f& vec)
{
	return b2Vec2(vec.x, vec.y);
}

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

		// box
		m_pBox = pScene->addEntityToScene();
		m_pBox->addComponent(new KCSprite(m_pBox, Vec2f(32, 32)));
		m_pBox->getTransform()->setTranslation(Vec2f(32, 32));
		m_pBox->getTransform()->setRotation(12.5f);
		m_pBox->getComponent<KCSprite>()->setShader(ASSET().getShader(L"default"));
		m_pBox->getComponent<KCSprite>()->setTexture(ASSET().getTexture(L"white"));
		m_pBox->getTransform()->setOrigin(Vec2f(16, 16));


		// floor
		m_pFloor = pScene->addEntityToScene();
		m_pFloor->addComponent(new KCSprite(m_pFloor, FloorBounds));
		m_pFloor->getTransform()->setTranslation(Vec2f(32, 300));
		m_pFloor->getComponent<KCSprite>()->setShader(ASSET().getShader(L"default"));
		m_pFloor->getComponent<KCSprite>()->setTexture(ASSET().getTexture(L"white"));
		m_pFloor->getTransform()->setOrigin(Vec2f(32, 16));
		//m_pFloor->getTransform()->setRotation(-45);


		{// falling box
			const float rotation = Radians(m_pBox->getTransform()->getRotation());

			b2BodyDef def;
			def.position.Set(32, 32);
			def.angle = rotation;
			def.type = b2_dynamicBody;
			m_pBody = world.CreateBody(&def);

			b2FixtureDef fixtureDef;
			b2MassData massData;
			m_shape.m_radius = 16;

			fixtureDef.friction = 0.2f;
			fixtureDef.restitution = 0.2;
			fixtureDef.shape = &m_shape;
			fixtureDef.density = 1;
			m_pBody->CreateFixture(&fixtureDef);


			m_pBody->SetActive(true);
			m_pBody->SetAwake(true);
			m_pBody->GetMassData(&massData);
			massData.mass = 1;
			m_pBody->SetMassData(&massData);

		}

		{// floor
			b2BodyDef def;
			def.position.Set(32, 300);
			def.type = b2_staticBody;
			m_pFloorBody = world.CreateBody(&def);

			m_floorShape.SetAsBox(FloorBounds.x / 2.0f, FloorBounds.y / 2.0f);
			m_pFloorBody->CreateFixture(&m_floorShape, 0.0f);
		}

		return KInitStatus::Success;
	}

	virtual void onEnterScene() override
	{
		m_pBox->getComponent<KCSprite>()->setColour(Colour::Red);
	}

	virtual void tick() override
	{
		if (KInput::Pressed(KKey::Space))
		{
			m_pBody->ApplyTorque(10000000, true);
		}
		b2MassData d;
		m_pBody->GetMassData(&d);

		float dt = KApplication::getApp()->getDeltaTime();
		world.Step(dt, VelocityIterations, PositionIterations);
		KCTransform* pBoxTrans = m_pBox->getTransformComponent();
		pBoxTrans->setTranslation(m_pBody->GetPosition().x, m_pBody->GetPosition().y);
		pBoxTrans->setRotation(m_pBody->GetAngle());

		ImGui::SFML::Update(*KApplication::getApp()->getRenderWindow(), sf::seconds(1.0f / (float)(KApplication::getApp()->getGameFPS())));
		ImGui::Begin("Physics Setup Editor");
		bool bDrop = false;
		bDrop = ImGui::Button("Drop box gravity?", Vec2i(90, 30));
		if (bDrop)
		{
			const float rotation = Radians(m_pBox->getTransform()->getRotation());
			m_pBody->SetTransform(b2Vec2(32, 32), rotation);
			world.SetGravity(Vec2ftob2Vec2(Vec2f(0, 9.81) * 10.0f));
			m_pBody->SetAwake(true);
		}

		ImGui::End();
	}

private:
	KEntity* m_pBox;
	KEntity* m_pFloor;

	b2Body* m_pBody;
	b2CircleShape m_shape;

	b2Body* m_pFloorBody;
	b2PolygonShape m_floorShape;

	int32 numberOfBoxes;
	std::array<KEntity*, 12> m_boxes;
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
	entity->addComponent(new imguicomp(entity));
	//DO STUFF WITH ENTITY HERE
	InitialiseSubmodules();

	RunApplication();

	ShutdownEngine();

	return 0;
}

