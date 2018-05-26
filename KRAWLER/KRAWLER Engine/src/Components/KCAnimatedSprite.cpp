#include "Components\KCAnimatedSprite.h"
#include "KApplication.h"

#include <SFML\System\Time.hpp>
#include <AssetLoader\KAssetLoader.h>

using namespace Krawler;
using namespace Krawler::Components;
using namespace Krawler::Animation;


KCAnimatedSprite::KCAnimatedSprite(Krawler::KEntity *pEntity, KAnimation * pAnimation)
	: KCRenderableBase(pEntity), m_bIsPlaying(false), m_bRepeat(false), m_animTimer(0.0f), m_frameIdx(0), m_pCurrentAnim(pAnimation)
{
	if (!pAnimation)
	{
		return;
	}

	m_verts = sf::VertexArray(sf::Quads, 4);

	setupVertArray();
	m_pTransformComponent = pEntity->getTransformComponent();
}

void Krawler::Components::KCAnimatedSprite::updateTextureRect(const Rectf & texRect)
{
	m_verts[0].texCoords = Vec2f(texRect.left, texRect.top);
	m_verts[1].texCoords = Vec2f(texRect.left + texRect.width, texRect.top);
	m_verts[2].texCoords = Vec2f(texRect.left + texRect.width, texRect.top + texRect.height);
	m_verts[3].texCoords = Vec2f(texRect.left, texRect.top + texRect.height);
}

void Krawler::Components::KCAnimatedSprite::setupVertArray()
{
	const Vec2f& bounds = m_pCurrentAnim->bounds;
	const Vec2f& frameData = m_pCurrentAnim->frameData[m_frameIdx];

	m_verts[0].position = Vec2f(0.0f, 0.0f);
	m_verts[1].position = Vec2f(m_pCurrentAnim->bounds.x, 0.0f);
	m_verts[2].position = Vec2f(m_pCurrentAnim->bounds.x, m_pCurrentAnim->bounds.y);
	m_verts[3].position = Vec2f(0.0f, m_pCurrentAnim->bounds.y);

	const Vec2f topLeftTex(bounds.x * frameData.x, bounds.y * frameData.y);

	m_verts[0].texCoords = topLeftTex;
	m_verts[1].texCoords = Vec2f(topLeftTex.x + bounds.x, topLeftTex.y);
	m_verts[2].texCoords = Vec2f(topLeftTex.x + bounds.x, topLeftTex.y + bounds.y);
	m_verts[3].texCoords = Vec2f(topLeftTex.x, topLeftTex.y + bounds.y);

	for (auto i = 0; i < 4; ++i)
	{
		m_verts[i].color = Colour::White;
	}

}

KRAWLER_API void Krawler::Components::KCAnimatedSprite::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	rStates.transform *= m_pTransformComponent->getTransform();
	rStates.shader = getShader();
	rStates.texture = m_pCurrentAnim->pTexture;
	rTarget.draw(m_verts, rStates);
}

void KCAnimatedSprite::tick()
{
	const float dt = KApplication::getApp()->getDeltaTime();
	if (!m_bIsPlaying) // if anim is not playing don't handle any of tick states
	{
		return;
	}

	m_animTimer += dt;
	if (m_animTimer > m_pCurrentAnim->frameTime)
	{
		const sf::Time frameTime = sf::seconds(m_pCurrentAnim->frameTime);
		const sf::Time currentFrameTime = sf::seconds(m_animTimer);

		const sf::Time ModuloCurrentTime = sf::microseconds(currentFrameTime.asMicroseconds() % frameTime.asMicroseconds());
		m_animTimer = ModuloCurrentTime.asSeconds();

		if (m_frameIdx + 1 < m_pCurrentAnim->frameData.size())
		{
			setFrameIndex(m_frameIdx + 1);
		}
		else
		{
			if (m_bRepeat)
			{
				setFrameIndex(0);
			}
			else
			{
				m_bIsPlaying = false;
			}
			m_animTimer = 0.0f;
		}
	}

}

KRAWLER_API Rectf Krawler::Components::KCAnimatedSprite::getOnscreenBounds() const
{
	return m_pTransformComponent->getTransform().transformRect(Rectf(Vec2f(0.0f, 0.0f), m_pCurrentAnim->bounds));
}

void KCAnimatedSprite::play()
{
	m_bIsPlaying = true;
}

void KCAnimatedSprite::pause()
{
	m_bIsPlaying = false;
}

void KCAnimatedSprite::stop()
{
	m_animTimer = 0.0f;
	m_bIsPlaying = false;
	setFrameIndex(0);
}

void KCAnimatedSprite::setFrameIndex(Krawler::int32 i)
{
	KCHECK(i < m_pCurrentAnim->frameData.size());
	m_frameIdx = i;
	Rectf rect;
	rect.width = m_pCurrentAnim->bounds.x;
	rect.height = m_pCurrentAnim->bounds.y;
	rect.left = m_pCurrentAnim->frameData[i].x * m_pCurrentAnim->bounds.x;
	rect.top = m_pCurrentAnim->frameData[i].y * m_pCurrentAnim->bounds.y;
	updateTextureRect(rect);
}

void KCAnimatedSprite::setAnimation(const std::wstring & animName)
{
	KAnimation* pAnim = KAssetLoader::getAssetLoader().getAnimation(animName);

	KCHECK(pAnim);

	m_pCurrentAnim = pAnim;
	setupVertArray();
	setFrameIndex(0);
	m_animTimer = 0.0f;
}
