#include "stdafx.h"
#include "KApplication.h"

using namespace Krawler;
using namespace sf;

void KApplication::setupApplication(const KApplicationInitialise & appInit)
{
	mp_rWindow = new RenderWindow;

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
		style |= (Style::Default & Style::Resize);
		break;

	case KWindowStyle::Windowed_Resizeable:
		style |= Style::Default;
		break;

	case KWindowStyle::Fullscreen:
		style |= Style::Fullscreen;
		break;
	}

	mp_rWindow->create(VideoMode(appInit.width, appInit.height), appInit.windowTitle, style);
	mp_rWindow->setFramerateLimit(m_gameFPS);
}

void KApplication::runApplication()
{
	m_elapsedClock.restart();
	Time lastTime(m_elapsedClock.getElapsedTime());
	Time currentTime;
	Time accumulator;
	Time time;
	sf::Clock deltaClock;


	while (mp_rWindow->isOpen())
	{
		m_gameDelta = deltaClock.getElapsedTime().asSeconds();
		Time frameTime;
		updateFrameTime(currentTime, lastTime, frameTime, accumulator);

		Event evnt;

		while (mp_rWindow->pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				mp_rWindow->close();
			}
		}

		if (frameTime > seconds(0.25f))
		{
			frameTime = seconds(0.25f);
		}

		while (accumulator.asSeconds() > m_physicsDelta)
		{
			//previousState = currentState;
			//Physics tick
			mp_rWindow->clear(Color::Red);
			mp_rWindow->display();

			time += seconds(m_physicsDelta);
			accumulator -= seconds(m_physicsDelta);
			KPrintf(L"Physics Tick\n");
		}
		KPrintf(L"Normal Tick\n");

		const float alpha = accumulator.asSeconds() / m_physicsDelta;

		mp_rWindow->clear(Color::Black);
		mp_rWindow->display();
	}
}

void Krawler::KApplication::cleanupApplication()
{
	KFREE(mp_rWindow);
}

float Krawler::KApplication::getElapsedTime() const
{
	return m_elapsedClock.getElapsedTime().asSeconds();
}

KApplication::KApplication()
{
}

inline void Krawler::KApplication::updateFrameTime(Time& currentTime, Time& lastTime, Time & frameTime, Time & accumulator) const
{
	currentTime = m_elapsedClock.getElapsedTime();

	frameTime = currentTime - lastTime;

	lastTime = currentTime;

	accumulator += frameTime;
}
