#include "stdafx.h"	
#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

using namespace Krawler;
using namespace Krawler::Renderer;

using namespace std;

Krawler::Renderer::KRenderer::~KRenderer()
{
	m_renderQueue.clear();
}

void Krawler::Renderer::KRenderer::addToRendererQueue(KGameObject * pGameObj)
{
	KCHECK(pGameObj);
	if (!pGameObj)
	{
		return;
	}

	m_renderQueue.push_back(pGameObj);
}

void Krawler::Renderer::KRenderer::render()
{
	sf::RenderWindow* const target = KApplication::getApplicationInstance()->getRenderWindow();

	sortByRenderLayer();

	target->clear();

	for (auto& obj : m_renderQueue)
	{
		target->draw(*obj);
	}

	target->display();
}

bool Krawler::Renderer::KRenderer::isInRenderQueue(KGameObject* pGameObj) const
{
	auto p = find(m_renderQueue.begin(), m_renderQueue.end(), pGameObj);
	return (p != m_renderQueue.end());
}

bool Krawler::Renderer::KRenderer::isInRenderQueue(const std::wstring & identifier) const
{
	bool found = false;
	for (auto& a : m_renderQueue)
	{
		if (a->getObjectName() == identifier)
		{
			found = true;
			break;
		}
	}
	return found;
}

void Krawler::Renderer::KRenderer::removeFromRenderQueue(KGameObject* pObj)
{
	m_renderQueue.erase(std::find(m_renderQueue.begin(), m_renderQueue.end(), pObj));
}

void Krawler::Renderer::KRenderer::sortByRenderLayer()
{
	std::sort(m_renderQueue.begin(), m_renderQueue.end(), [](KGameObject* objA, KGameObject* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}
