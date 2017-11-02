#include "stdafx.h"
#include "KApplication.h"

using namespace Krawler;
using namespace sf;

void KApplication::setupApplication(const KApplicationInitialise & appInit)
{
	mp_rWindow = new RenderWindow;

	m_targetFPS = appInit.maxFps;
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
	mp_rWindow->setFramerateLimit(m_targetFPS);
}

void KApplication::runApplication()
{
	Clock currentTime;
	Clock newTime;
	
	Time

	while (mp_rWindow->isOpen())
	{
		Event evnt;
		while (mp_rWindow->pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				mp_rWindow->close();
			}
		}

		mp_rWindow->clear(Color::Black);
		mp_rWindow->display();
		
	}
}

KApplication::KApplication()
{
}
