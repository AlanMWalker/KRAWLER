#ifndef KCTRANSFORM_H
#define KCTRANSFORM_H

#include "Krawler.h"
#include "KComponent.h"
#include "KEntity.h"
#include <SFML\Graphics\Transform.hpp>

namespace Krawler
{
	namespace Components
	{
		/*
		World & local transform manipulating component.
		All entities are given a transform component upon construction.

		Use case:
		int main()
		{
		KEntity entity;
		auto pTransform = entity.getComponent<KCTransform>();
		KCHECK(pTransform);
		pTransform->setScale(1.0f, 1.0f); // will set entity

		return 0;
		}
		*/
		class KCTransform : public KComponentBase
		{
		public:

			//Param: Pointer to entity transform is attatched to 
			//Return: n/a 
			//Info: 
			KRAWLER_API KCTransform(KEntity* pEntity);
			KRAWLER_API ~KCTransform() = default;

			//Param: n/a
			//Return: n/a
			//Info: Will update the entire transform object 
			KRAWLER_API virtual void tick() override;

			//Param: n/a 
			//Return: Transform
			//Info: Returns a reference to the internally stored transform entity
			KRAWLER_API const sf::Transform& getTransform();

			//Return: n/a
			//Info: Set the entitiy this transform is parented to 
			KRAWLER_API void setParent(KEntity* pEntity);

			//Param: float angle of rotation 
			//Return: n/a 
			//Info: set the local rotation of the entity (in degrees)
			KRAWLER_API void setRotation(float angleInDeg);

			//Param: Vec2f origin point/float x, float y
			//Return: n/a
			//Info: set the origin for rotation and scale and translation (local space to the entity)
			KRAWLER_API void setOrigin(const Vec2f& origin);
			KRAWLER_API void setOrigin(float x, float y);

			//Param: Vec2f translation/float transX, transY
			//Return: n/a
			//Info: set translation of entity 
			KRAWLER_API void setTranslation(const Vec2f& trans);
			KRAWLER_API void setTranslation(float dx, float dy);

			//Param: n/a
			//Return: Vec2f position
			//Info: get world position of entity
			KRAWLER_API const Vec2f getPosition() const;

			//Param: n/a 
			//Return: float rotation 
			//Info: get world rotation of entity (in degrees)
			KRAWLER_API float getRotation() const;

			//Param: n/a 
			//Return: vec2f origin
			//Info: get local origin of entity (in degrees)
			KRAWLER_API const Vec2f& getOrigin()const;

			//Param: Vec2f scale/float scaleX, float scale Y
			//Return: n/a
			//Info: set scale of entity  
			KRAWLER_API void setScale(const Vec2f& scale);
			KRAWLER_API void setScale(float x, float y);

			//Param: n/a
			//Return: Vec2f scale
			//Info: get scale of entity
			KRAWLER_API Vec2f getScale() const;

			//Param: Vec2f moveVector/float xMove, yMove
			//Return: n/a 
			//Info: Translate the entity by Vector(xMove,yMove)
			KRAWLER_API void move(float dx, float dy);
			KRAWLER_API void move(const Vec2f& trans);

			//Param: float angle 
			//Return: n/a
			//Info: rotate the entity by angle passed (in degrees)
			KRAWLER_API void rotate(float angleInDeg);

			//Param: Vec2f scale/scale x, scale y 
			//Return: n/a
			//Info: scale the entity by vec(scaleX,scaleY)
			KRAWLER_API void scale(const Vec2f& scale);
			KRAWLER_API void scale(float sx, float sy);

		private:

			void reconstructTransform();

			KCTransform* m_pParentTransform;

			sf::Transform m_transform;
			sf::Transform m_combinedWithParentTransform;

			float m_rotation;

			Vec2f m_origin;
			Vec2f m_scale;
			Vec2f m_trans;

			Vec2f m_worldPos;
			Vec2f m_worldScale;
			float m_worldRot;

			bool m_bHasParent;
			bool m_bUpdateTransform;
		};
	}


}
#endif 
