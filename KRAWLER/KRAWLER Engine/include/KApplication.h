#ifndef KAPPLICATION_H
#define KAPPLICATION_H

#include "Krawler.h"
#include "Renderer\KRenderer.h"
#include "Input\KInput.h"

#include <SFML\Graphics\RenderWindow.hpp> 
#include <SFML\System\Clock.hpp> 

namespace Krawler
{
	//Forward Declerations
	namespace LogicState
	{
		class KLogicStateDirector;
	}

	enum KWindowStyle : uint8
	{
		Windowed_Fixed_Size,
		Windowed_Resizeable,
		Fullscreen
	};

	struct KApplicationInitialise
	{
		uint32 width; // Width of the window 
		uint32 height; // Height of the window
		uint32 gameFps = 60; // Game FPS (render & update)
		uint32 physicsFps = 100; // fps for physics engine to update at
		std::wstring windowTitle; //title of the window
		bool consoleWindow; // Is the console window enabled on your build
		KWindowStyle windowStyle = Windowed_Fixed_Size; // Window style
	};

	class KApplication
	{
	public:

		KRAWLER_API static KApplication* const getApplicationInstance()
		{
			static KApplication* pApplication = new KApplication();

			return pApplication;
		}
		KRAWLER_API ~KApplication() = default;

		KRAWLER_API KInitStatus initialiseStateDirector();

		KRAWLER_API void setupApplication(const KApplicationInitialise& appInit);
		KRAWLER_API void runApplication();
		KRAWLER_API void cleanupApplication();

		KRAWLER_API sf::RenderWindow* const getRenderWindow() { return mp_renderWindow; }
		KRAWLER_API Krawler::LogicState::KLogicStateDirector* const getLogicStateDirector() { return mp_logicStateDirector; }
		KRAWLER_API Krawler::Renderer::KRenderer* const getRenderer() { return mp_renderer; }

		KRAWLER_API float getElapsedTime() const;
		KRAWLER_API float getDelta() const { return m_gameDelta; }
		KRAWLER_API float getPhysicsDelta() const { return m_physicsDelta; }


	private:

		KApplication();
		inline void updateFrameTime(sf::Time& currentTime, sf::Time& lastTime, sf::Time& frameTime, sf::Time& accumulator) ;

		sf::RenderWindow* mp_renderWindow = nullptr;
		Krawler::LogicState::KLogicStateDirector* mp_logicStateDirector = nullptr;
		Krawler::Renderer::KRenderer* mp_renderer = nullptr;

		uint32 m_gameFPS;
		uint32 m_physicsFPS;

		int32 m_frames = 0;

		sf::Clock m_elapsedClock;

		float m_gameDelta = 0.0f;
		float m_physicsDelta = 0.0f;
		
		bool mb_isFirstUpdate = true;
	};
}
#endif
