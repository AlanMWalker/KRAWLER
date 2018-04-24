#include "Input\KInput.h"
#include "KApplication.h"

using namespace Krawler;
using namespace Krawler::Input;

sf::RenderWindow* KInput::mp_window = nullptr;

std::set<sf::Keyboard::Key> KInput::m_keysJustPressed;
std::set<sf::Keyboard::Key> KInput::m_keysPressed;
std::set<sf::Keyboard::Key> KInput::m_keysJustReleased;
std::set<sf::Mouse::Button> KInput::m_mouseJustPressed;
std::set<sf::Mouse::Button> KInput::m_mousePressed;
std::set<sf::Mouse::Button> KInput::m_mouseJustReleased;
std::wstring KInput::m_textEntered;

sf::Vector2i KInput::m_mousePosition;
sf::Vector2i KInput::m_mouseDelta;
float KInput::m_mouseScrollDelta = 0.0f;
bool KInput::mb_mouseLocked = false;

void KInput::SetWindow(sf::RenderWindow* window)
{
	KCHECK(KInput::mp_window == nullptr);
	KInput::mp_window = window;
}

void KInput::Update()
{
	m_keysJustPressed.clear();
	m_keysJustReleased.clear();
	m_mouseJustPressed.clear();
	m_mouseJustReleased.clear();
	m_mouseScrollDelta = 0.0f;
	m_textEntered.clear();

	if (mp_window)
	{
		sf::Vector2i newMousePosition = sf::Mouse::getPosition(*mp_window);
		m_mouseDelta = newMousePosition - m_mousePosition;
		if (mb_mouseLocked)
		{
			m_mousePosition = sf::Vector2i(mp_window->getSize()) / 2;
			sf::Mouse::setPosition(m_mousePosition, *mp_window);
		}
		else
		{
			m_mousePosition = newMousePosition;
		}
	}
}

void KInput::HandleEvent(const sf::Event& evt)
{
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		EventKeyPressed(evt.key.code);
		break;
	case sf::Event::KeyReleased:
		EventKeyReleased(evt.key.code);
		break;
	case sf::Event::MouseButtonPressed:
		EventMouseButtonPressed(evt.mouseButton.button);
		break;
	case sf::Event::MouseButtonReleased:
		EventMouseButtonReleased(evt.mouseButton.button);
		break;
	case sf::Event::TextEntered:
		EventTextEntered(evt.text.unicode);
		break;
	case sf::Event::MouseWheelScrolled:
		EventMouseScrollMoved(evt.mouseWheelScroll.delta);
		break;
	default:
		//TODO log warning here.. 
		//LOG_WARNING(std::to_string(evt.type) + " is not a valid event type for KInput to handle");
		break;
	}
}

void KInput::EventKeyPressed(sf::Keyboard::Key key)
{
	m_keysJustPressed.insert(key);
	m_keysPressed.insert(key);
}

void KInput::EventKeyReleased(sf::Keyboard::Key key)
{
	auto removeIt = m_keysPressed.find(key);
	if (removeIt != m_keysPressed.end())
		m_keysPressed.erase(removeIt);
	m_keysJustReleased.insert(key);
}

void KInput::EventMouseButtonPressed(sf::Mouse::Button button)
{
	m_mouseJustPressed.insert(button);
	m_mousePressed.insert(button);
}

void KInput::EventMouseButtonReleased(sf::Mouse::Button button)
{
	auto removeIt = m_mousePressed.find(button);
	if (removeIt != m_mousePressed.end())
		m_mousePressed.erase(removeIt);
	m_mouseJustReleased.insert(button);
}

void KInput::EventMouseScrollMoved(float delta)
{
	m_mouseScrollDelta = delta;
}

void KInput::EventTextEntered(sf::Uint32 charCode)
{
	m_textEntered += (wchar_t)charCode;
}

bool KInput::JustPressed(sf::Keyboard::Key key)
{
	return ((m_keysJustPressed.find(key) != m_keysJustPressed.end()));
}

bool KInput::Pressed(sf::Keyboard::Key key)
{
	return (m_keysPressed.find(key) != m_keysPressed.end());
}

bool KInput::JustReleased(sf::Keyboard::Key key)
{
	return (m_keysJustReleased.find(key) != m_keysJustReleased.end());
}

bool KInput::MouseJustPressed(sf::Mouse::Button button)
{
	return (m_mouseJustPressed.find(button) != m_mouseJustPressed.end());
}

bool KInput::MousePressed(sf::Mouse::Button button)
{
	return (m_mousePressed.find(button) != m_mousePressed.end());
}

bool KInput::MouseJustReleased(sf::Mouse::Button button)
{
	return (m_mouseJustReleased.find(button) != m_mouseJustReleased.end());
}

void KInput::SetMouseLocked(bool mouseLocked)
{
	KInput::mb_mouseLocked = mouseLocked;
	if (mouseLocked)
	{
		m_mousePosition = sf::Vector2i(mp_window->getSize()) / 2;
		sf::Mouse::setPosition(m_mousePosition, *mp_window);
	}
}

Vec2f Krawler::Input::KInput::GetMouseWorldPosition()
{
	//auto pRenderWindow = KApplication::getApp()->getRenderWindow();
	Vec2f worldPos = mp_window->mapPixelToCoords(m_mousePosition);

	return worldPos;
}
