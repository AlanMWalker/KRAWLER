#ifndef KAPPLICATION_H
#define KAPPLICATION_H

#include "Krawler.h"
#include "Renderer\KRenderer.h"
#include "Input\KInput.h"
#include "KScene.h"
#include <fstream>
#include <SFML\Graphics\RenderWindow.hpp> 
#include <SFML\System\Clock.hpp> 

#include <mutex>
#include <atomic>
#include <functional>

//Lowest FPS for Physics & Game: 24
//Highest FPS for Physics & Game: 80

#define GET_SCENE() Krawler::KApplication::getApp()->getCurrentScene()

namespace Krawler
{
	enum KWindowStyle : uint8
	{
		Windowed_Fixed_Size,
		Windowed_Resizeable,
		Fullscreen
	};

	struct KApplicationInitialise
	{
		KRAWLER_API KApplicationInitialise(bool loadPreset = false)
		{
			if (loadPreset)
				loadFromEnginePreset();
		}

		uint32 width; // Width of the window 
		uint32 height; // Height of the window
		uint32 gameFps = 60; // Game FPS (render & update)
		uint32 physicsFps = 100; // fps for physics engine to update at
		std::wstring windowTitle; //title of the window
		bool consoleWindow; // Is the console window enabled on your build
		KWindowStyle windowStyle = Windowed_Fixed_Size; // Window style

		KRAWLER_API void loadFromEnginePreset();

	};
	KRAWLER_API std::wifstream& operator >> (std::wifstream& os, KApplicationInitialise& data);

	class KApplication
	{
	public:

		KRAWLER_API static KApplication* const getApp()
		{
			static KApplication* pApplication = new KApplication();

			return pApplication;
		}

		KRAWLER_API ~KApplication() = default;

		KInitStatus initialiseScenes();

		KRAWLER_API void setupApplication(const KApplicationInitialise& appInit);
		KRAWLER_API void runApplication();
		KRAWLER_API void cleanupApplication();

		//TODO do not expose this function in the dll
		KRAWLER_API sf::RenderWindow* const getRenderWindow() { return m_pRenderWindow; }
		KRAWLER_API Krawler::Renderer::KRenderer* const getRenderer() { return m_pRenderer; }
		KRAWLER_API Krawler::KSceneDirector& getSceneDirector() { return m_sceneDirector; }
		KRAWLER_API Krawler::Physics::KPhysicsWorld* const getPhysicsWorld() { return &m_physicsWorld; }
		KRAWLER_API Krawler::KScene* const getCurrentScene() { return m_sceneDirector.getCurrentScene(); }

		KRAWLER_API float getElapsedTime() const;
		KRAWLER_API float getGameFPS() const { return m_gameFPS; }
		KRAWLER_API float getDeltaTime() const { return m_gameDelta; }
		KRAWLER_API float getPhysicsDelta() const { return m_physicsDelta; }

		KRAWLER_API Vec2u getWindowSize() const;

		KRAWLER_API void closeApplication();

		KRAWLER_API void subscribeToEventQueue(std::function<void(const sf::Event&)> function);

		static std::mutex& getMutexInstance() { return s_mutex; }

	private:

		KApplication();

		void fixedStep();

		__forceinline void updateFrameTime(sf::Time& currentTime, sf::Time& lastTime, sf::Time& frameTime, sf::Time& accumulator);
		__forceinline void outputFPS(const sf::Time& currentTime, sf::Time& fpsLastTime);

		sf::RenderWindow* m_pRenderWindow = nullptr;
		Krawler::Renderer::KRenderer* m_pRenderer = nullptr;
		Krawler::KSceneDirector m_sceneDirector;
		Physics::KPhysicsWorld m_physicsWorld;

		uint32 m_gameFPS;
		uint32 m_physicsFPS;

		int32 m_frames = 0;

		sf::Clock m_elapsedClock;

		float m_gameDelta = 0.0f;
		float m_physicsDelta = 0.0f;

		bool m_bIsFirstUpdate = true;
		bool m_bHasFocus = true;
		Vec2f m_viewSize;
		static std::mutex s_mutex;
		std::vector< std::function<void(const sf::Event&)>> m_eventQueueCallbacks;
	};
}

#define GET_APP() Krawler::KApplication::getApp()
#define GET_DIRECTOR() Krawler::KApplication::getApp()->getSceneDirector()
#endif
