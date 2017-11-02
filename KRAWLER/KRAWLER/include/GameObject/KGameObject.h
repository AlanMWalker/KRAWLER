#ifndef KGAME_OBJECT_H
#define KGAME_OBJECT_H

#include "Krawler.h"

namespace Krawler
{
	class KGameObject : public sf::Transformable, public sf::Drawable
	{
	public:

		KGameObject();
		explicit KGameObject(const Vec2f& size);
		~KGameObject();

		virtual void draw(sf::RenderTarget& renderTarget, sf::RenderStates renderStates) const;

		sf::FloatRect getAlignedGlobalBounds() const;
		sf::FloatRect getFixedGlobalBounds() const;
		sf::FloatRect getLocalBounds() const;

		sf::Int32 getRenderLayer() const { return m_renderLayer; }
		bool isGameObjectActive() const { return mb_isGOActive; }

		void setSize(const Vec2f& size);
		void setTexture(sf::Texture* texPtr, bool resetTextureRect = true);
		void setTextureRect(const sf::IntRect& textureRect);
		void setFillColour(const sf::Color& colour);
		void setName(const std::string& objName);
		void setRenderLayer(sf::Int32 renderLayer) { m_renderLayer = renderLayer; }
		void setObjectInactive() { mb_isGOActive = false; }
		void setObjectActive() { mb_isGOActive = true; }

	private:

		void initialiseVertexArray();
		void updateTextureCoords();

		sf::Texture* mp_texture = nullptr;

		sf::VertexArray m_vertArray;
		sf::IntRect m_texRect = sf::IntRect();

		std::wstring m_objName = TEXT("GO-") + GenerateUUID();

		Vec2f m_size = Vec2f(1.0f, 1.0f);

		sf::Int32 m_renderLayer = 0;

		bool mb_isGOActive = true;
	};
}

#endif 
