#ifndef KAPPLICATION_H
#define KAPPLICATION_H

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
		uint32 width; // Width of the window 
		uint32 height; // Height of the window
		uint16 maxFps = 60;
		std::string windowTitle; //title of the window
		bool consoleWindow; // Is the console window enabled on your build
		KWindowStyle windowStyle = Windowed_Fixed_Size; 
	};

	class KApplication
	{
	public:

		static KApplication* const getApplicationInstance()
		{
			static KApplication* pApplication = new KApplication();

			return pApplication;
		}

		~KApplication();

		void setupApplication(const KApplicationInitialise& appInit);
		void runApplication();
	private:
		KApplication();

		sf::RenderWindow* mp_rWindow = nullptr;

		uint32 m_targetFPS;
	};
}
#endif
