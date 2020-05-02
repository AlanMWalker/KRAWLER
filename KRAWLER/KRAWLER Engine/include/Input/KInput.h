#ifndef KINPUT_H
#define KINPUT_H

#include "Krawler.h"

//STL includes
#include <set>
#include <string>

//SFML includes
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Window\Mouse.hpp>
#include <SFML\Window\Joystick.hpp>

//TODO change set to list

namespace Krawler
{
	namespace Input
	{
		using KKey = sf::Keyboard::Key;
		using KMouseButton = sf::Mouse::Button;
		
		class KInput
		{
		public:

			KRAWLER_API static void SetWindow(sf::RenderWindow* window);
			KRAWLER_API static void Update();
			KRAWLER_API static void HandleEvent(const sf::Event& evt);

			KRAWLER_API static bool JustPressed(sf::Keyboard::Key key); //TODO Prevent repeat key firing
			KRAWLER_API static bool Pressed(sf::Keyboard::Key key);
			KRAWLER_API static bool JustReleased(sf::Keyboard::Key key);
			KRAWLER_API static bool MouseJustPressed(sf::Mouse::Button button);
			KRAWLER_API static bool MousePressed(sf::Mouse::Button button);
			KRAWLER_API static bool MouseJustReleased(sf::Mouse::Button button);
			KRAWLER_API static bool JoystickJustPressed(unsigned int button);
			KRAWLER_API static bool JoystickJustReleased(unsigned int button);

			KRAWLER_API static void SetMouseLocked(bool mouseLocked);

			// Returns mouse position in screen space
			KRAWLER_API static Krawler::Vec2i GetMousePosition() { return m_mousePosition; }
			
			// Returns mouse move delta
			KRAWLER_API static Krawler::Vec2i GetMouseDelta() { return m_mouseDelta; }
			
			// Returns mouse position in world space
			KRAWLER_API static Krawler::Vec2f GetMouseWorldPosition();

			KRAWLER_API static float GetMouseScrollDelta() { return m_mouseScrollDelta; }

			KRAWLER_API static std::wstring GetTextEntered() { return m_textEntered; }

		private:
			static void EventKeyPressed(sf::Keyboard::Key key);
			static void EventKeyReleased(sf::Keyboard::Key key);
			static void EventMouseButtonPressed(sf::Mouse::Button button);
			static void EventMouseButtonReleased(sf::Mouse::Button button);
			static void EventMouseScrollMoved(float delta);
			static void EventTextEntered(sf::Uint32 charCode);
			static void EventJoystickButtonPressed(unsigned int);
			static void EventJoystickButtonReleased(unsigned int);

			static sf::RenderWindow* mp_window;

			static std::set<sf::Keyboard::Key> m_keysJustPressed;
			static std::set<sf::Keyboard::Key> m_keysPressed;
			static std::set<sf::Keyboard::Key> m_keysJustReleased;

			static sf::Vector2i m_mousePosition;
			static sf::Vector2i m_mouseDelta;

			static std::set<sf::Mouse::Button> m_mouseJustPressed;
			static std::set<sf::Mouse::Button> m_mousePressed;
			static std::set<sf::Mouse::Button> m_mouseJustReleased;

			static std::set<unsigned int> m_joystickJustPressed;
			static std::set<unsigned int> m_joystickJustReleased;

			static std::wstring m_textEntered;

			static float m_mouseScrollDelta;

			static bool mb_mouseLocked;
		};
	}
}
#endif
