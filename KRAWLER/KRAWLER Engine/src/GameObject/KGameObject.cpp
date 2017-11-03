	
#include "GameObject\KGameObject.h"

#define VERT_PER_QUAD 4

using namespace sf;
using namespace Krawler;

KGameObject::KGameObject()
{
	initialiseVertexArray();
}

KGameObject::KGameObject(const Vec2f& size) :
	m_size(size)
{
	initialiseVertexArray();
}

KGameObject::~KGameObject()
{
	m_vertArray.clear();
}

void KGameObject::draw(sf::RenderTarget & renderTarget, sf::RenderStates renderStates) const
{
	renderStates.texture = mp_texture;
	renderStates.transform *= getTransform();

	renderTarget.draw(m_vertArray, renderStates);
}

sf::FloatRect KGameObject::getAlignedGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::FloatRect KGameObject::getFixedGlobalBounds() const
{
	FloatRect fixedGlobalBounds;

	Vec2f topLeft = getTransform().transformPoint(Vec2f(0.0, 0.0f));
	fixedGlobalBounds.left = topLeft.x;
	fixedGlobalBounds.top = topLeft.y;
	fixedGlobalBounds.width = m_size.x * getScale().x;
	fixedGlobalBounds.height = m_size.y * getScale().y;

	return fixedGlobalBounds;
}

sf::FloatRect KGameObject::getLocalBounds() const
{
	FloatRect localBounds;

	localBounds.left = 0.0f;
	localBounds.top = 0.0f;
	localBounds.width = m_size.x;
	localBounds.height = m_size.y;

	return localBounds;
}

void KGameObject::setSize(const Vec2f & size)
{
	m_size = size;
	initialiseVertexArray();
}

void KGameObject::setTexture(Texture * texPtr, bool resetTextureRect)
{
	if (!texPtr)
	{
		return;
	}

	mp_texture = texPtr;
	if (resetTextureRect)
	{
		IntRect defaultRect;
		defaultRect.top = defaultRect.left = 0;
		defaultRect.width = static_cast<signed>(texPtr->getSize().x);
		defaultRect.height = static_cast<signed>(texPtr->getSize().x);
		setTextureRect(defaultRect);
	}
}

void KGameObject::setTextureRect(const IntRect& textureRect)
{
	m_texRect = textureRect;
}

void KGameObject::setFillColour(const sf::Color & colour)
{
	m_vertArray[0].color = colour;
	m_vertArray[1].color = colour;
	m_vertArray[2].color = colour;
	m_vertArray[3].color = colour;
}

void KGameObject::setName(const std::string & objName)
{
}

//private functions 

void KGameObject::initialiseVertexArray()
{
	if (m_vertArray.getVertexCount() != VERT_PER_QUAD)
	{
		m_vertArray.resize(VERT_PER_QUAD);
	}

	m_vertArray.setPrimitiveType(Quads);

	m_vertArray[0].position = Vec2f(0.0f, 0.0f);
	m_vertArray[1].position = Vec2f(m_size.x, 0.0f);
	m_vertArray[2].position = m_size;
	m_vertArray[3].position = Vec2f(0.0f, m_size.y);

}

void KGameObject::updateTextureCoords()
{
	const Vec2f castedPosition = Vec2f((float)m_texRect.left, (float)m_texRect.top);
	const Vec2f castedSize = Vec2f((float)m_texRect.width, (float)m_texRect.height);

	m_vertArray[0].texCoords = castedPosition;
	m_vertArray[1].texCoords = (castedPosition + Vec2f(castedSize.x, 0.0f));
	m_vertArray[2].texCoords = (castedPosition + castedSize);
	m_vertArray[3].texCoords = (castedPosition + Vec2f(0.0f, castedSize.y));
}