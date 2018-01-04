
#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

#include <SFML\Graphics.hpp>
#include <AssetLoader\KAssetLoader.h>
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
	m_screenText.clear();
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
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();
	mp_defaultFont.loadFromFile("res\\seriphim.ttf");

	while (target->isOpen())
	{
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

		for (auto& text : m_screenText)
		{
			sf::Text t(text.second.getString(), mp_defaultFont);
			t.setCharacterSize(text.second.getCharacterSize());
			t.setPosition(screenToWorld(text.first));
			target->draw(t);
		}

		target->display();
	}
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

KRAWLER_API void Krawler::Renderer::KRenderer::clearRenderQueue()
{
	m_renderQueue.clear();
	m_screenText.clear();
}

KRAWLER_API void Krawler::Renderer::KRenderer::setActiveTiledMap(TiledMap::KTiledMap * pTiledMap)
{
	if (pTiledMap != nullptr)
	{
		mp_tiledMap = pTiledMap;
		mb_hasTiledMap = true;
	}
}

void Krawler::Renderer::KRenderer::generateSpriteList()
{
	m_sprites.clear();
	auto const pCurrentScene = KApplication::getApp()->getCurrentScene();
	KCHECK(pCurrentScene);

	auto entityList = pCurrentScene->getEntitiyList();

	for (int i = 0; i < pCurrentScene->getNumbrOfEntitiesAllocated(); ++i)
	{
		KEntity& entity = entityList[i];
		if (!entity.isEntitiyInUse())
		{
			continue;
		}
		auto pSprite = entity.getComponent<Components::KCSprite>();
		if (!pSprite)
		{
			continue;
		}
		m_sprites.push_back(pSprite);
	}
}

void Krawler::Renderer::KRenderer::sortByRenderLayer()
{
	std::sort(m_sprites.begin(), m_sprites.end(), [](Components::KCSprite* objA, Components::KCSprite* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}

void Krawler::Renderer::KRenderer::defaultRender()
{
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();

	//sortByRenderLayer();

	//if (mb_hasTiledMap)
	//{
	//	target->draw(*mp_tiledMap);
	//}
	//
	//for (auto& obj : m_renderQueue)
	//{
	//	if (!obj->isGameObjectActive())
	//	{
	//		continue;
	//	}
	//	target->draw(*obj);
	//}
	generateSpriteList();
	sortByRenderLayer();

	for (auto& sprite : m_sprites)
	{
		target->draw(*sprite);
	}

}

void Krawler::Renderer::KRenderer::raycastRender()
{

}

Vec2f Krawler::Renderer::KRenderer::screenToWorld(const Vec2i & vec) const
{
	return KApplication::getApp()->getRenderWindow()->mapPixelToCoords(vec);
}
