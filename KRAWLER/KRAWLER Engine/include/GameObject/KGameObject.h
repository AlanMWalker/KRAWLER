#ifndef KGAME_OBJECT_H
#define KGAME_OBJECT_H

#include <SFML\Graphics.hpp>
#include "Krawler.h"

namespace Krawler
{
	namespace Physics
	{
		class KPhysicsBody;
	}

	class KGameObject : public sf::Transformable, public sf::Drawable
	{
	public:

		KRAWLER_API KGameObject();
		KRAWLER_API explicit KGameObject(const Vec2f& size);
		KRAWLER_API ~KGameObject();

		KRAWLER_API virtual void draw(sf::RenderTarget& renderTarget, sf::RenderStates renderStates) const;

		KRAWLER_API sf::FloatRect getAlignedGlobalBounds() const;
		KRAWLER_API sf::FloatRect getFixedGlobalBounds() const;
		KRAWLER_API sf::FloatRect getLocalBounds() const;
		KRAWLER_API Vec2f getHalfLocalBounds() const;

		//Fix to account for non 0,0 origins
		KRAWLER_API Vec2f getCentrePosition() const;

		KRAWLER_API sf::Int32 getRenderLayer() const { return m_renderLayer; }
		KRAWLER_API bool isGameObjectActive() const { return mb_isGOActive; }

		KRAWLER_API void setSize(const Vec2f& size);
		KRAWLER_API void setTexture(sf::Texture* texPtr, bool resetTextureRect = true);
		KRAWLER_API void setTextureRect(const sf::IntRect& textureRect);
		KRAWLER_API void setFillColour(const sf::Color& colour);
		KRAWLER_API void setName(const std::wstring& objName);
		KRAWLER_API void setRenderLayer(sf::Int32 renderLayer) { m_renderLayer = renderLayer; } // Increasing render layer is closer to screen, decreasing is further from screen
		KRAWLER_API void setObjectInactive() { mb_isGOActive = false; }
		KRAWLER_API void setObjectActive() { mb_isGOActive = true; }
		KRAWLER_API void setPhysicsBody(Physics::KPhysicsBody* pBody);

		KRAWLER_API std::wstring getObjectName() const { return m_objName; }
		KRAWLER_API Physics::KPhysicsBody* const getPhysicsBody() { return mp_physicsBody; }

		const sf::VertexArray& getVertArray() const { return m_vertArray; }

	private:

		void initialiseVertexArray();
		void updateTextureCoords();

		sf::Texture* mp_texture = nullptr;
		Physics::KPhysicsBody* mp_physicsBody = nullptr;

		sf::VertexArray m_vertArray;
		sf::IntRect m_texRect = sf::IntRect();

		std::wstring m_objName = KTEXT("GO-") + GenerateUUID();

		Vec2f m_size = Vec2f(1.0f, 1.0f);

		sf::Int32 m_renderLayer = 0;

		bool mb_isGOActive = true;
	};
}

#endif 
