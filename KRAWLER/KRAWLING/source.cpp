#include "stdafx.h"
#include "vld.h"

#include <Krawler.h>
#include <KApplication.h>
#include <LogicState\KLogicStateDirector.h>
#include <LogicState\KLogicState.h>
#include <Physics\KPhysicsScene.h>
#include <Utilities\KDebug.h>

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Input;

class TestState : public KLogicState
{
public:

	TestState() {}

	~TestState() {}

	Krawler::Physics::KPhysicsBody* body = nullptr;
	Krawler::Physics::KPhysicsBody* pBodyJustAdded = nullptr;
	bool bBodyJustAdded = false;

	virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser) override
	{
		auto status = KLogicState::setupState(initaliser);
		if (status != Success)
		{
			return status;
		}

		mp_physicsScene->setGravity(Vec2f(0.0f, 9.81));

		auto a = addGameObject(Vec2f(KApplication::getApplicationInstance()->getWindowSize().x, 10));

		a->setFillColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));

		a->setPosition(0.0f, KApplication::getApplicationInstance()->getWindowSize().y - 10);
		body = mp_physicsScene->addBody(a, 10000.0f, false);
		body->setIsStatic(true);
		body->setRestitution(0.9f);
		a->setPhysicsBody(body);

		mp_physicsScene->setPercentageCorrection(0.2f);
		mp_physicsScene->setSlop(0.09f);

		return Success;
	}

	virtual void tick() override
	{
		if (KInput::MouseJustPressed(sf::Mouse::Left))
		{
			const Vec2f mousePos(KInput::GetMousePosition());
			const Vec2f randSize(Maths::RandFloat(10, 20), Maths::RandFloat(10, 20));
			KGameObject* const a = addGameObject(randSize);
			a->setFillColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));

			a->setOrigin(a->getHalfLocalBounds());
			a->setPosition(mousePos);
			pBodyJustAdded = mp_physicsScene->addBody(a, 1.0f);
			a->setPhysicsBody(pBodyJustAdded);
			pBodyJustAdded->setRestitution(0.9f);
			bBodyJustAdded = true;
			Krawler::KPrintf(KTEXT("%d bodies in scene\n"), mp_physicsScene->getPhysicsBodyCount());
		}
		if (KInput::JustPressed(sf::Keyboard::Key::Escape))
		{
			KApplication::getApplicationInstance()->closeApplication();
		}
	}

	virtual void fixedTick() override
	{
		KLogicState::fixedTick(); // tick physics engine 
		body->resetForce();

		if (bBodyJustAdded)
		{
			//pBodyJustAdded->setRestitution(0.0f);
			pBodyJustAdded->setMass(Maths::RandFloat(1.0f, 10.0f));
			//pBodyJustAdded->applyTorque(9000.0f);
			pBodyJustAdded->applyForce(Vec2f(Maths::RandFloat(-5000, 5000), 0.f));

			bBodyJustAdded = false;
		}
	}
};

int main(void)
{
	KApplicationInitialise initApp;
	initApp.consoleWindow = true;
	initApp.width = 640;
	initApp.height = 480;
	initApp.windowStyle = Windowed_Fixed_Size;
	initApp.gameFps = 30;
	initApp.physicsFps = 100;
	initApp.windowTitle = KTEXT("Hello World!");

	StartupEngine(&initApp);

	TestState* state = new TestState;
	auto application = KApplication::getApplicationInstance();

	KLogicStateInitialiser initState;
	initState.bIsPhysicsEngineEnabled = true;

	application->getLogicStateDirector()->registerLogicState(dynamic_cast<KLogicState*>(state), &initState);
	application->getLogicStateDirector()->setActiveLogicState(initState.stateIdentifier);

	InitialiseStateDirector();

	RunApplication();
	ShutdownEngine();

	return 0;
}

