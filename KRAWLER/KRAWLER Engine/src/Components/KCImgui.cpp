#include "Components/KCImgui.h"
#include "KApplication.h"
//sfml imgui required headers
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <limits>
using namespace Krawler;
using namespace Krawler::Components;

KCImgui::KCImgui(KEntity * pEntity)
	: KCRenderableBase(pEntity)
{

}

KCImgui::~KCImgui()
{

}

KInitStatus KCImgui::init()
{
	ImGui::SFML::Init(*KApplication::getApp()->getRenderWindow());
	return KInitStatus::Success;
}

void KCImgui::tick()
{
	static bool bFirstUpdate = true;
	sf::RenderWindow* const pRenderWindow = KApplication::getApp()->getRenderWindow();
	const float dt = 1.0f / KApplication::getApp()->getGameFPS();

	if (bFirstUpdate)
	{
		ImGui::SFML::Update(*pRenderWindow, sf::seconds(dt));
		bFirstUpdate = false;
	}


	if (m_imguiState == KImguiState::DrawnLastFrame)
	{
		ImGui::SFML::Update(*pRenderWindow, sf::seconds(dt));
		m_imguiState = Updated;
	}
}

void KCImgui::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	if (m_imguiState == KImguiState::BeginEndDone)
	{
		ImGui::SFML::Render(rTarget);
	}
}

Rectf KCImgui::getOnscreenBounds() const
{
	static Rectf NO_BOUNDS{ 0,std::numeric_limits<float>::max(),0,0 };
	return NO_BOUNDS;
}

void KCImgui::begin(const std::string& name)
{
	if (m_imguiState == KImguiState::Updated)
	{
		ImGui::Begin(name.c_str());
		m_imguiState = KImguiState::BeginDone;
	}
}

void KCImgui::end()
{
	if (m_imguiState == KImguiState::BeginDone)
	{
		ImGui::End();
		m_imguiState = KImguiState::BeginEndDone;
	}
}

void KCImgui::postRenderEvent()
{
	if (m_imguiState == KImguiState::BeginEndDone)
	{
		m_imguiState = KImguiState::DrawnLastFrame;
	}
}
