#include "Renderer\KRenderer.h"
#include "KApplication.h"

#include <algorithm>

#include <SFML\Graphics.hpp>
#include <AssetLoader\KAssetLoader.h>

using namespace Krawler;
using namespace Krawler::Renderer;

using namespace std;

KRenderer::KRenderer()
	: m_renderingType(KRendererType::Default)
{
}

KRenderer::~KRenderer()
{
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
