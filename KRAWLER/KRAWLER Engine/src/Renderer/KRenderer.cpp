	
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
	KCHECK(pGameObj);
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
	KCHECK(pObj);
	if (!pObj)
	{
		return;
	}

	if (!isInRenderQueue(pObj))
	{
		return;
	}

	m_renderQueue.erase(std::find(m_renderQueue.begin(), m_renderQueue.end(), pObj));
}

void Krawler::Renderer::KRenderer::removeFromRenderQueue(const std::wstring & identifier)
{
	if (!isInRenderQueue(identifier))
	{
		return;
	}

	int32 idx = 0;
	bool found = false;
	while (idx < KCAST(int32, m_renderQueue.size()))
	{
		if (m_renderQueue[idx]->getObjectName() == identifier)
		{
			found = true;
			break;
		}
		++idx;
	}

	if (!found)
	{
		return;
	}
	auto a = std::find(m_renderQueue.begin(), m_renderQueue.end(), m_renderQueue[idx]);
	m_renderQueue.erase(a);
}

void Krawler::Renderer::KRenderer::sortByRenderLayer()
{
	std::sort(m_renderQueue.begin(), m_renderQueue.end(), [](KGameObject* objA, KGameObject* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}
