#include "KApplication.h"
#include <future>

using namespace Krawler;
using namespace Krawler::Renderer;
using namespace sf;
using namespace std;

std::mutex KApplication::s_mutex;


KInitStatus Krawler::KApplication::initialiseScenes()
{
	KINIT_CHECK(m_sceneDirector.initScenes());
	return KInitStatus::Success;
}

void KApplication::setupApplication(const KApplicationInitialise& appInit)
{
	m_pRenderWindow = new RenderWindow;
	m_pRenderWindow->setKeyRepeatEnabled(false);
	//setup game fps values
	m_gameFPS = Maths::Clamp(24u, 80u, appInit.gameFps);
	m_physicsFPS = Maths::Clamp(24u, 80u, appInit.physicsFps);

	//setup the target update fps for the physics engine
	if (m_physicsFPS != 0)
	{
		m_physicsDelta = 1.0f / KCAST(float, appInit.physicsFps);
	}

	int32 style = 0;

	switch (appInit.windowStyle)
	{
	default:
	case KWindowStyle::Windowed_Fixed_Size:
		style |= (Style::Close | Style::Titlebar);
		break;

	case KWindowStyle::Windowed_Resizeable:
		style |= Style::Default;
		break;

	case KWindowStyle::Fullscreen:
		style |= Style::Fullscreen;
		break;
	}

	sf::ContextSettings settings;
	m_pRenderWindow->create(VideoMode(appInit.width, appInit.height), appInit.windowTitle, style, settings);
	m_pRenderWindow->setFramerateLimit(m_gameFPS);
	m_viewSize.x = static_cast<float>(appInit.width);
	m_viewSize.y = static_cast<float>(appInit.height);
	m_pRenderWindow->setView(sf::View(Rectf(0, 0, static_cast<float>(appInit.width), static_cast<float>(appInit.height))));

	//m_pRenderer = new KRenderer;

	// TODO check physics world return
	m_physicsWorld.initialiseWorld();

	Input::KInput::SetWindow(m_pRenderWindow);
}

void KApplication::runApplication()
{
	m_elapsedClock.restart();
	Time lastTime(m_elapsedClock.getElapsedTime()); // Updated with previous frames time
	Time fpsLastTime; //For FPS counter 
	Time currentTime; //Constantly updated time value
	Time accumulator; //time accumulator for fixed step 
	Time time;
	sf::Clock deltaClock;


	//m_pRenderWindow->setActive(false);
	//std::thread	rThread(&KRenderer::render, m_pRenderer);
	//std::thread pThread(&KApplication::fixedStep, this);
	while (m_pRenderWindow->isOpen())
	{
		m_gameDelta = deltaClock.restart().asSeconds();
		Time frameTime;
		updateFrameTime(currentTime, lastTime, frameTime, accumulator);

		outputFPS(currentTime, fpsLastTime);

		Event sfmlEvent;

		Input::KInput::Update();

		while (m_pRenderWindow->pollEvent(sfmlEvent))
		{
			if (sfmlEvent.type == Event::Closed)
			{
				m_pRenderWindow->close();
			}
			if (sfmlEvent.type == Event::GainedFocus)
			{
				m_bHasFocus = true;
			}
			if (sfmlEvent.type == Event::LostFocus)
			{
				m_bHasFocus = false;
			}

			if (sfmlEvent.type == Event::Resized)
			{
				//Rectf v{ 0, 0, (float)sfmlEvent.size.width , (float)sfmlEvent.size.height };
				//m_pRenderWindow->setView(View(v));
			}

			//if (m_bHasFocus)
			{
				Input::KInput::HandleEvent(sfmlEvent);
			}

			for (auto& func : m_eventQueueCallbacks)
			{
				func(sfmlEvent);
			}
		}

		if (frameTime > seconds(m_physicsDelta * 4))
		{
			frameTime = seconds(m_physicsDelta * 4);
		}

		if (accumulator > seconds(m_physicsDelta * 2))
		{
			accumulator = seconds(m_physicsDelta * 2);
		}

		//if (m_bHasFocus)
		{
			while (accumulator.asSeconds() > m_physicsDelta)
			{
				//previousState = currentState;
				//Physics tick
				m_physicsWorld.stepWorld(m_physicsDelta);
				m_sceneDirector.fixedTickActiveScene();
				time += seconds(m_physicsDelta);
				accumulator -= seconds(m_physicsDelta);
			}
		}

		++m_frames;

		const float alpha = accumulator.asSeconds() / m_physicsDelta;
		//DO: KScene renderer lerp
		//mp_logicStateDirector->physicsLerp(alpha);

		//if (m_bHasFocus)
		{

			m_sceneDirector.tickActiveScene();
			m_overlord.tick();

		}
		m_pRenderer.render();

		const float EXTRA_FPS_BUMP = 0;
		const float timeInSec = deltaClock.getElapsedTime().asSeconds();
		const float sleepTime = (1.0f / (m_gameFPS + EXTRA_FPS_BUMP)) - timeInSec;
		//sf::sleep(sf::seconds(sleepTime));
		this_thread::sleep_for(chrono::milliseconds(static_cast<int32>(sleepTime * 1000)));
	}
	//rThread.join();
	//pThread.join();
}

void Krawler::KApplication::cleanupApplication()
{
	m_physicsWorld.cleanupWorld();
	m_sceneDirector.cleanupScenes();
	KFREE(m_pRenderWindow);
	//KFREE(m_pRenderer);
}

float Krawler::KApplication::getElapsedTime() const
{
	return m_elapsedClock.getElapsedTime().asSeconds();
}

KRAWLER_API Vec2u Krawler::KApplication::getWindowSize() const
{
	return m_pRenderWindow->getSize();
}

KRAWLER_API void Krawler::KApplication::closeApplication()
{
	m_pRenderWindow->close();
}

void Krawler::KApplication::subscribeToEventQueue(std::function<void(const sf::Event&)> function)
{
	m_eventQueueCallbacks.push_back(function);
}

KApplication::KApplication()
{
}

//Runs on pThread
void Krawler::KApplication::fixedStep()
{
	Clock deltaClockFixedStep;
	Clock elapsed;
	Time accumulator; //time accumulator for fixed step 
	Time lastTime;
	Time currentTime;
	Time time;

	while (m_pRenderWindow->isOpen())
	{
		//KPrintf(KTEXT("Physics Text\n"));
		//sf::sleep( )
		lastTime = currentTime;
		currentTime = elapsed.getElapsedTime();
		Time frameTime;

		frameTime = currentTime - lastTime;

		const int32 idealDT = (int32)((1.0f / m_gameFPS) * 1000.0f);
		accumulator += frameTime;

		if (frameTime > seconds(m_physicsDelta * 4))
		{
			frameTime = seconds(m_physicsDelta * 4);
		}

		if (accumulator > seconds(m_physicsDelta * 2))
		{
			accumulator = seconds(m_physicsDelta * 2);
		}

		if (m_bHasFocus)
		{
			while (accumulator.asSeconds() > m_physicsDelta)
			{
				//previousState = currentState;
				//Physics tick
				//m_physicsWorld.fixedTick();
				m_sceneDirector.fixedTickActiveScene();
				time += seconds(m_physicsDelta);
				accumulator -= seconds(m_physicsDelta);
			}
		}


		const int32 dtInMS = deltaClockFixedStep.restart().asMilliseconds();
		this_thread::sleep_for(chrono::milliseconds(idealDT - dtInMS));
		//KPrintf(L"DT: %f \n", 1.0f / ((float)(dtInMS + (idealDT - dtInMS)) / 1000.0f));
	}
}

inline void Krawler::KApplication::updateFrameTime(Time& currentTime, Time& lastTime, Time& frameTime, Time& accumulator)
{
	currentTime = m_elapsedClock.getElapsedTime();
	if (m_bIsFirstUpdate)
	{
		lastTime = currentTime;
	}

	frameTime = currentTime - lastTime;

	lastTime = currentTime;

	accumulator += frameTime;
}

void Krawler::KApplication::outputFPS(const sf::Time& currentTime, sf::Time& fpsLastTime)
{
	if (m_bIsFirstUpdate)
	{
		m_frames = 0;
		m_bIsFirstUpdate = false;
		fpsLastTime = currentTime;
		return;
	}

	if (currentTime - fpsLastTime > seconds(0.25f) && m_frames > 50)
	{
		const float fps = (KCAST(float, m_frames) / (currentTime - fpsLastTime).asSeconds());

		const float ms = 1.0f / fps;
		KPrintf(KTEXT("FPS: %f(%f ms per frame)\n"), fps, ms);
		//m_fpsText.setString(std::to_string(ms) + " ms/frame\n " + "FPS: " + std::to_string(fps));
		//
		fpsLastTime = currentTime;
		m_frames = 0;
	}
}
// -- KApplicationInitialise  --
void Krawler::KApplicationInitialise::loadFromEnginePreset()
{
	//LOAD PRESET ENGINE CONFIG
	wifstream engConfig;

	engConfig.open("kconfig.cfg", ios::in);
	if (engConfig.fail())
	{
		KPrintf(KTEXT("Error! Couldn't find engine preset config"));
		//default to 640x480
		width = 640;
		height = 480;
		return;
	}
	KApplicationInitialise temp;
	engConfig >> temp;
	(*this) = temp;

	engConfig.close();
}

std::wifstream& Krawler::operator >> (std::wifstream& os, KApplicationInitialise& data)
{
	wchar_t str[100];

	os >> data.width;
	os >> data.height;
	os >> data.gameFps;
	os >> data.physicsFps;
	os.get();
	//os >> data.windowTitle;
	os.getline(str, 100 * sizeof(wchar_t));
	data.windowTitle = str;
	os >> data.consoleWindow;
	int style = 0;
	os >> style;
	data.windowStyle = (KWindowStyle)style;
	return os;
};