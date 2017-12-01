
#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

using namespace Krawler;
using namespace Krawler::Renderer;

using namespace std;

Krawler::Renderer::KRenderer::KRenderer()
	: m_renderingType(KRendererType::Default), m_renderQueue(0), mp_tiledMap(nullptr)
{

}

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
	target->clear();

	switch (m_renderingType)
	{
	default:
	case Default:
		defaultRender();
		break;
	case Raycast:
		raycastRender();
		break;
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

KRAWLER_API void Krawler::Renderer::KRenderer::setActiveTiledMap(TiledMap::KTiledMap * pTiledMap)
{
	if (pTiledMap != nullptr)
	{
		mp_tiledMap = pTiledMap;
		mb_hasTiledMap = true;
	}
}

void Krawler::Renderer::KRenderer::sortByRenderLayer()
{
	std::sort(m_renderQueue.begin(), m_renderQueue.end(), [](KGameObject* objA, KGameObject* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}

void Krawler::Renderer::KRenderer::defaultRender()
{
	sf::RenderWindow* const target = KApplication::getApplicationInstance()->getRenderWindow();

	sortByRenderLayer();

	if (mb_hasTiledMap)
	{
		target->draw(*mp_tiledMap);
	}

	for (auto& obj : m_renderQueue)
	{
		target->draw(*obj);
	}
}

void Krawler::Renderer::KRenderer::raycastRender()
{
	/* Hackjob raycast test*/
	Vec2i playerGridPos(5, 5);
	Vec2f playerPos(playerGridPos * 32);
	Vec2f playerDir(1.0f, 0.0f);
	Vec2f plane(0.0f, 0.66f);
	Vec2f rayPos(playerPos);
	const float w = (float)KApplication::getApplicationInstance()->getRenderWindow()->getSize().x;
	const float h = (float)KApplication::getApplicationInstance()->getRenderWindow()->getSize().y;

	for (int x = 0; x < w; ++x)
	{
		float cameraX = 2 * x / w - 1;
		Vec2f sideDist;
		Vec2f deltaDist;
		Vec2f rayDir(playerDir + plane * cameraX);
		Vec2i mapPos(rayPos.x, rayPos.y);

		deltaDist.x = sqrtf(1 + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x));
		deltaDist.y = sqrtf(1 + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y));

		Vec2i stepDirection;

		bool hit = false;
		int side;

		if (rayDir.x != 0)
		{
			stepDirection.x = rayDir.x / fabs(rayDir.x);

			if (stepDirection.x < 0)
			{
				sideDist.x = (rayPos.x - mapPos.x) * deltaDist.x;
			}
			else
			{
				sideDist.x = (mapPos.x + 1.0 - rayPos.x) * deltaDist.x;

			}
		}
		if (rayDir.y != 0)
		{
			stepDirection.y = rayDir.y / fabs(rayDir.y);

			if (stepDirection.y < 0)
			{
				sideDist.y = (rayPos.y - mapPos.y) * deltaDist.y;
			}
			else
			{
				sideDist.y = (mapPos.y + 1.0 - rayPos.y) * deltaDist.y;
			}
		}

		while (hit == false)
		{
			if (sideDist.x < sideDist.y)
			{
				sideDist.x += deltaDist.x;
				mapPos.x += stepDirection.x;
				side = 0;
			}
			else
			{
				sideDist.y += deltaDist.y;
				mapY += stepDirection.y;
			}
		}


	}

}
