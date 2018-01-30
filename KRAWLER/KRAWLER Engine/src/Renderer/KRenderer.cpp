
#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

#include <SFML\Graphics.hpp>
#include <AssetLoader\KAssetLoader.h>
using namespace Krawler;
using namespace Krawler::Renderer;

using namespace std;

KRenderer::KRenderer()
	: m_renderingType(KRendererType::Default), m_renderQueue(0), mp_tiledMap(nullptr)
{
}

KRenderer::~KRenderer()
{
	m_renderQueue.clear();
	m_screenText.clear();
}

void KRenderer::addToRendererQueue(KGameObject * pGameObj)
{
	KCHECK(pGameObj);
	if (!pGameObj)
	{
		return;
	}

	m_renderQueue.push_back(pGameObj);
}

void KRenderer::render()
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

bool KRenderer::isInRenderQueue(KGameObject* pGameObj) const
{
	KCHECK(pGameObj);
	auto p = find(m_renderQueue.begin(), m_renderQueue.end(), pGameObj);
	return (p != m_renderQueue.end());
}

bool KRenderer::isInRenderQueue(const std::wstring & identifier) const
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

void KRenderer::removeFromRenderQueue(KGameObject* pObj)
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

void KRenderer::removeFromRenderQueue(const std::wstring & identifier)
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

KRAWLER_API void KRenderer::clearRenderQueue()
{
	m_renderQueue.clear();
	m_screenText.clear();
}

KRAWLER_API void KRenderer::setActiveTiledMap(TiledMap::KTiledMap * pTiledMap)
{
	if (pTiledMap != nullptr)
	{
		mp_tiledMap = pTiledMap;
		mb_hasTiledMap = true;
	}
}

KRAWLER_API void KRenderer::addTiledMap(int32 renderOrder, TiledMap::KTiledMap * pTiledMap)
{
	if (!pTiledMap)
	{
		return;
	}

	if (!mb_hasTiledMap)
	{
		mb_hasTiledMap = true;
	}

	TileMapRenderItem item;
	item.first = renderOrder;
	item.second = pTiledMap;

	m_tiledMaps.push_back(item);

	std::sort(m_tiledMaps.begin(), m_tiledMaps.end(),
		[](const TileMapRenderItem& lhs, const TileMapRenderItem& rhs) -> bool
	{
		return lhs.first < rhs.first;
	});
}

void KRenderer::generateSpriteList()
{
	KApplication::getMutexInstance().lock();

	m_sprites.clear();
	auto const pCurrentScene = KApplication::getApp()->getCurrentScene();
	KCHECK(pCurrentScene);
	auto entityList = pCurrentScene->getEntityList();

	for (int i = 0; i < (signed)pCurrentScene->getNumbrOfEntitiesAllocated(); ++i)
	{
		KEntity& entity = entityList[i];
		if (!entity.isEntityInUse())
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
	KApplication::getMutexInstance().unlock();

}

void KRenderer::sortByRenderLayer()
{
	std::sort(m_sprites.begin(), m_sprites.end(), [](Components::KCSprite* objA, Components::KCSprite* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}

void KRenderer::defaultRender()
{
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();

	if (mb_hasTiledMap)
	{
		for (auto& tiledMapRenderItem : m_tiledMaps)
		{
			target->draw(*tiledMapRenderItem.second);
		}
	}

	generateSpriteList();
	sortByRenderLayer();

	for (auto& sprite : m_sprites)
	{
		target->draw(*sprite);
	}

}

void KRenderer::raycastRender()
{

}

Vec2f KRenderer::screenToWorld(const Vec2i & vec) const
{
	return KApplication::getApp()->getRenderWindow()->mapPixelToCoords(vec);
}
