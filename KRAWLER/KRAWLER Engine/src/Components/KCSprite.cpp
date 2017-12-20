#include "Components\KCSprite.h"

//#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics.hpp>

using namespace Krawler;
using namespace Krawler::Components;

KCSprite::KCSprite(KEntity * pEntity, const Vec2f& size)
	:KComponentBase(pEntity), m_size(size), m_pTexture(nullptr), m_pTransform(nullptr), m_renderLayer(0)
{
}

KInitStatus KCSprite::init()
{
	m_vertexArray = sf::VertexArray(sf::Quads, 4);

	m_vertexArray[0].position = Vec2f(0.0f, 0.0f);
	m_vertexArray[1].position = Vec2f(m_size.x, 0.0f);
	m_vertexArray[2].position = Vec2f(m_size.x, m_size.y);
	m_vertexArray[3].position = Vec2f(0.0f, m_size.y);

	m_pTransform = getEntity()->getComponent<KCTransform>();
	KCHECK(m_pTransform);
	if (!m_pTransform)
	{
		return KInitStatus::Failure;
	}
	return KInitStatus::Success;
}

void KCSprite::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	rStates.transform *= m_pTransform->getTransform();
	//rStates.texture = m_pTexture;

	rTarget.draw(m_vertexArray, rStates);
}

KRAWLER_API void KCSprite::setColour(const Colour & col)
{
	for (int i = 0; i < 4; ++i)
	{
		m_vertexArray[i].color = col;
	}
}
