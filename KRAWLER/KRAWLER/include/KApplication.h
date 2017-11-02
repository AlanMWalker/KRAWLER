#ifndef KAPPLICATION_H
#define KAPPLICATION_H


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
		std::string windowTitle; //title of the window
		bool consoleWindow; // Is the console window enabled on your build
		KWindowStyle windowStyle = Windowed_Fixed_Size; // Window style
	};

	class KApplication
	{
	public:

		static KApplication* const getApplicationInstance()
		{
			static KApplication* pApplication = new KApplication();

			return pApplication;
		}
		~KApplication() = default;

		void setupApplication(const KApplicationInitialise& appInit);
		void runApplication();
		void cleanupApplication();

		sf::RenderWindow* const getRenderWindow() { return mp_rWindow; }
		Krawler::LogicState::KLogicStateDirector* const getLogicStateDirector() { return mp_logicStateDirector; }
		
		float getElapsedTime() const;
		float getDelta() const { return m_gameDelta; }

	private:

		KApplication();
		inline void updateFrameTime(sf::Time& currentTime, sf::Time& lastTime, sf::Time& frameTime, sf::Time& accumulator) const;

		sf::RenderWindow* mp_rWindow = nullptr;
		Krawler::LogicState::KLogicStateDirector* mp_logicStateDirector = nullptr;


		uint32 m_gameFPS;
		uint32 m_physicsFPS;

		sf::Clock m_elapsedClock;

		float m_gameDelta = 0.0f;
		float m_physicsDelta = 0.0f;
	};
}
#endif
