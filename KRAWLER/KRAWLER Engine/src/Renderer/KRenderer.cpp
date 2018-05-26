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

void KRenderer::render()
{
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();

	while (target->isOpen())
	{
		target->clear();
		defaultRender();

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

void KRenderer::generateRenderableList()
{
	KApplication::getMutexInstance().lock();

	m_renderable.clear();
	auto const pCurrentScene = KApplication::getApp()->getCurrentScene();
	KCHECK(pCurrentScene);
	auto entityList = pCurrentScene->getEntityList();

	for (int i = 0; i < CHUNK_POOL_SIZE; ++i)
	{
		if (!entityList[i].allocated)
		{
			continue;
		}

		KEntity& entity = entityList[i].entity;
		if (!entity.isEntityInUse())
		{
			continue;
		}
		auto pSprite = entity.getComponent<Components::KCRenderableBase>();
		if (!pSprite)
		{
			continue;
		}
		m_renderable.push_back(pSprite);
	}
	KApplication::getMutexInstance().unlock();
}

void KRenderer::sortByRenderLayer()
{
	std::sort(m_renderable.begin(), m_renderable.end(), [](Components::KCRenderableBase* objA, Components::KCRenderableBase* objB)
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

	generateRenderableList();
	sortByRenderLayer();
	const int32 size = m_renderable.size();
	for (auto& renderable : m_renderable)
	{
		target->draw(*renderable);
	}
}

Vec2f KRenderer::screenToWorld(const Vec2i & vec) const
{
	return KApplication::getApp()->getRenderWindow()->mapPixelToCoords(vec);
}
