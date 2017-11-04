#ifndef KPHYSICS_BODY_H
#define KPHYSICS_BODY_H

//Game Includes
#include "Krawler.h"
#include "Physics\KPhysicsStructs.h"

namespace Krawler
{
	//Forward declerations 
	class KGameObject;

	enum KPhysicsBodyShapeType : Krawler::uint32
	{
		Rectangle,
		Circle,
		ConvexPoly, //TODO add support for convex poly (SAT)
		ConcavePoly //TODO add support for concave (hull building algorithm) 
	};

	namespace Physics
	{
		struct PhysicsState
		{
			Vec2f v; //velocity
		};
		struct Derivative
		{
			Vec2f dx;//dx/dt
			Vec2f dv; //dv/dt
		};

		class KPhysicsBody
		{

		public:

			KRAWLER_API KPhysicsBody(float mass, bool subjectToGravity, KGameObject* gameObject);
			KRAWLER_API ~KPhysicsBody();

			//mutators
			KRAWLER_API void step(float delta, float pixelsToMetres);

			KRAWLER_API void applyForce(Vec2f force)
			{
				m_force += force;
			}

			KRAWLER_API void setMass(float m);
			KRAWLER_API void resetForce() { m_force = Vec2f{ 0.f, 0.f }; }
			KRAWLER_API void resetVelocity() { m_velocity = Vec2f{ 0.f, 0.f }; }
			KRAWLER_API void setGameObject(KGameObject* go) { mp_gameObject = go; } // Set the pointer to the shape of this body
			KRAWLER_API void setSubjectToGravity(const bool b) { mb_actsUnderGravity = b; } //set if the body is subject to gravity (Will respond to masses in the scene)
			KRAWLER_API void setVelocity(Vec2f v) { m_velocity = v; }; //set the velocity of the body( to kick the body off at a velocity at the start)
			KRAWLER_API void setRestitution(float e) { m_materialData.restitution = e; } //set the restitution of this body

			KRAWLER_API void setBodyUnusued() {
				mb_isBodyInUse = false;
				resetVelocity();
				resetForce();
			} // set a body as being unused 

			KRAWLER_API void moveBody(Vec2f translation);

			KRAWLER_API void setBodyUsed() { mb_isBodyInUse = true; } // set a body as in use
			//void addCollidedBody(KPhysicsBody* b);

			//accessors
			KRAWLER_API const KGameObject& getGameObject() const { return *mp_gameObject; }
			KRAWLER_API bool isSubjectToGravity() const { return mb_actsUnderGravity; } //is this body subject to gravity
			KRAWLER_API Vec2f getVelocity() const { return m_velocity; } //Get the velocity of this body 
			KRAWLER_API Vec2f getForce() const { return m_force; } //Get the total force currently acting upon the body
			KRAWLER_API float getMass() const { return m_massData.mass; }  //Access mass
			KRAWLER_API float getInverseMass() const { return m_massData.invMass; } // Access the inverse mass of the body (which is stored)
			KRAWLER_API KPhysicsBodyShapeType getShapeType() const { return m_shapeType; }


			KRAWLER_API bool isBodyInUse() const { return mb_isBodyInUse; }
			KRAWLER_API const MaterialData& getMaterialData() { return m_materialData; }
			KRAWLER_API const std::wstring getGameObjectName() const;

		private:

			KGameObject* mp_gameObject = nullptr;

			Vec2f m_velocity;
			Vec2f m_force;

			MassData m_massData;
			MaterialData m_materialData;

			KPhysicsBodyShapeType m_shapeType = KPhysicsBodyShapeType::Rectangle;

			bool mb_actsUnderGravity;
			bool mb_isBodyInUse = true; //Is this body currently being used
		};
	} //end physics
}//end krawler
#endif 
