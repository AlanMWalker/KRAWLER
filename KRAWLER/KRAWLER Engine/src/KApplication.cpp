
#include "KApplication.h"
#include "LogicState\KLogicStateDirector.h"

using namespace Krawler;
using namespace Krawler::LogicState;
using namespace Krawler::Renderer;
using namespace sf;

KRAWLER_API KInitStatus Krawler::KApplication::initialiseStateDirector()
{
	return mp_logicStateDirector->initaliseLogicStates();
}

void KApplication::setupApplication(const KApplicationInitialise & appInit)
{
	mp_renderWindow = new RenderWindow;

	//setup game fps values
	m_gameFPS = appInit.gameFps;
	m_physicsFPS = appInit.physicsFps;

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

	mp_renderWindow->create(VideoMode(appInit.width, appInit.height), appInit.windowTitle, style);
	mp_renderWindow->setFramerateLimit(m_gameFPS);

	mp_logicStateDirector = new KLogicStateDirector;
	mp_renderer = new KRenderer;

	Input::KInput::SetWindow(mp_renderWindow);

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

	bool bHasFocus = true;

	while (mp_renderWindow->isOpen())
	{

		m_gameDelta = deltaClock.getElapsedTime().asSeconds();
		Time frameTime;
		updateFrameTime(currentTime, lastTime, frameTime, accumulator);

		outputFPS(currentTime, fpsLastTime);

		Event evnt;

		Input::KInput::Update();

		while (mp_renderWindow->pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				mp_renderWindow->close();
			}
			if (evnt.type == Event::GainedFocus)
			{
				bHasFocus = true;
			}
			if (evnt.type == Event::LostFocus)
			{
				bHasFocus = false;
			}
			if (bHasFocus)
			{
				Input::KInput::HandleEvent(evnt);
			}
		}

		if (frameTime > seconds(m_physicsDelta * 4))
		{
			frameTime = seconds(m_physicsDelta * 4);
		}

		while (accumulator.asSeconds() > m_physicsDelta)
		{
			//previousState = currentState;
			//Physics tick
			mp_logicStateDirector->fixedTick();
			time += seconds(m_physicsDelta);
			accumulator -= seconds(m_physicsDelta);
		}

		++m_frames;

		const float alpha = accumulator.asSeconds() / m_physicsDelta;
		mp_logicStateDirector->physicsLerp(alpha);

		if (bHasFocus)
		{
			mp_logicStateDirector->tickActiveLogicState();
		}

		mp_renderer->render();

	}
}

void Krawler::KApplication::cleanupApplication()
{
	mp_logicStateDirector->cleanupLogicStateDirector();
	KFREE(mp_logicStateDirector);
	KFREE(mp_renderWindow);
	KFREE(mp_renderer);
}

float Krawler::KApplication::getElapsedTime() const
{
	return m_elapsedClock.getElapsedTime().asSeconds();
}

KRAWLER_API Vec2u Krawler::KApplication::getWindowSize() const
{
	return mp_renderWindow->getSize();
}

KRAWLER_API void Krawler::KApplication::closeApplication()
{
	mp_renderWindow->close();
}

KApplication::KApplication()
{
}

inline void Krawler::KApplication::updateFrameTime(Time& currentTime, Time& lastTime, Time & frameTime, Time & accumulator)
{
	currentTime = m_elapsedClock.getElapsedTime();

	frameTime = currentTime - lastTime;

	lastTime = currentTime;

	accumulator += frameTime;
}

void Krawler::KApplication::outputFPS(const sf::Time & currentTime, sf::Time & fpsLastTime)
{
	if (mb_isFirstUpdate)
	{
		m_frames = 0;
		mb_isFirstUpdate = false;
		fpsLastTime = currentTime;
		return;
	}

	if (currentTime - fpsLastTime > seconds(0.25f) && m_frames > 10)
	{
		const float fps = (KCAST(float, m_frames) / (currentTime - fpsLastTime).asSeconds());

		const float ms = 1.0f / fps;
		KPrintf(KTEXT("FPS: %f( %f ms per frame)\n"), fps, ms);
		//m_fpsText.setString(std::to_string(ms) + " ms/frame\n " + "FPS: " + std::to_string(fps));
		//
		fpsLastTime = currentTime;
		m_frames = 0;
	}
}
