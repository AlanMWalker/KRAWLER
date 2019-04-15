#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

#include <SFML\Graphics.hpp>
#include "AssetLoader\KAssetLoader.h"

using namespace Krawler;
using namespace Krawler::Renderer;

using namespace std;

KRenderer::KRenderer()
	: m_renderingType(KRendererType::Default), m_sortType(KRenderSortType::ZOrderSort)
{

}

KRenderer::~KRenderer()
{
	m_screenText.clear();
}

void KRenderer::render()
{
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();
	target->setActive(true);

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

void KRenderer::generateRenderableList()
{

	m_renderablesVector.clear();
	m_splitMapVec.clear();
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
		m_renderablesVector.push_back(pSprite);
	}


	for (auto renderable : m_renderablesVector)
	{
		KCHECK(renderable);
		Components::KCTileMapSplit* pSplitMap = dynamic_cast<Components::KCTileMapSplit*>(renderable);
		if (pSplitMap)
		{
			m_splitMapVec.push_back(pSplitMap);
		}
	}

	for (auto pSplit : m_splitMapVec)
	{
		auto& mapLines = pSplit->getHorizontalTileLines();
		for (auto& line : mapLines)
		{
			m_renderablesVector.push_back((Components::KCRenderableBase*)(&line));
		}
		m_renderablesVector.erase(std::find(m_renderablesVector.begin(), m_renderablesVector.end(), pSplit));
	}

}

void KRenderer::sortByRenderLayer()
{
	std::sort(m_renderablesVector.begin(), m_renderablesVector.end(), [](Components::KCRenderableBase* objA, Components::KCRenderableBase* objB)
	{
		return objA->getRenderLayer() < objB->getRenderLayer();
	});
}

void Krawler::Renderer::KRenderer::sortByZOrder()
{
	std::sort(m_renderablesVector.begin(), m_renderablesVector.end(), [](Components::KCRenderableBase* objA, Components::KCRenderableBase* objB) -> bool
	{
		auto objABounds = objA->getOnscreenBounds();
		auto objBBounds = objB->getOnscreenBounds();

		return (objABounds.top + objABounds.height) < (objBBounds.top + objBBounds.height);
	});
}

void KRenderer::defaultRender()
{
	sf::RenderWindow* const target = KApplication::getApp()->getRenderWindow();

	generateRenderableList();
	switch (m_sortType)
	{
	case KRenderSortType::ZOrderSort:
		sortByZOrder(); //@Remember to do impl for this function
		break;
	case KRenderSortType::LayerSort:
	default:
		sortByRenderLayer();
	}
	const int32 size = m_renderablesVector.size();
	for (auto& pSplit : m_splitMapVec)
	{
		target->draw(*pSplit);
	}

	for (auto& renderable : m_renderablesVector)
	{
		target->draw(*renderable);
		renderable->postRenderEvent();
	}

}

Vec2f KRenderer::screenToWorld(const Vec2i & vec) const
{
	return KApplication::getApp()->getRenderWindow()->mapPixelToCoords(vec);
}
