#ifndef KCBODY_H
#define KCBODY_H

// KRAWLER Includes
#include "Krawler.h"
#include "KComponent.h"

// Box2D Includes
#include "box2d/box2d.h"

namespace Krawler
{
	namespace Components
	{

		enum class BodyType : int32
		{
			Static_Body = 0,
			Kinematic_Body,
			Dynamic_Body
		};
		/*
		Physics Body Definition
		*/
		struct KBodyDef
		{
			KBodyDef() :
				bBullet(false), gravityScale(1.0f), bAwake(true),
				bodyType(BodyType::Static_Body), angle(0.0f),
				linearVelocity(0.0f, 0.0f), angularVelocity(0.0f),
				bFixedRotation(false), linearDamping(0.0f),
				angularDamping(0.0f), bActive(true), bAllowSleep(true)

			{
			}
			~KBodyDef() = default;
			bool bBullet;
			float gravityScale;
			bool bAwake;
			bool bAllowSleep;
			BodyType bodyType;
			float angle;
			Vec2f position;
			Vec2f linearVelocity;
			float angularVelocity;
			bool bActive;
			bool bFixedRotation;
			float linearDamping;
			float angularDamping;

		};

		/*
		Physics Material Definition
		NOTE:
		The following from Box2D's 'b2Fixture Def' have not been
		translated into KRAWLER:
		- isSensor
		- collision filtering
		- shape
		- userData (may not support)
		*/
		struct KMatDef
		{
			KMatDef()
				: density(0.0f), friction(0.1f), restitution(0.0f)
			{
			}
			~KMatDef() = default;

			float density;
			float friction;
			float restitution;
		};

		class KCBody :
			public KComponentBase
		{
		public:

			KRAWLER_API KCBody(KEntity& entity, const Vec2f& bounds);
			KRAWLER_API KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef);
			KRAWLER_API KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef, const KMatDef& matDef);

			KRAWLER_API ~KCBody() = default;

			// Overrides from component base
			KRAWLER_API virtual KInitStatus init() override;
			KRAWLER_API virtual void fixedTick() override;
			KRAWLER_API virtual void onExitScene() override; 

			// @return Material definition this body was created with
			KRAWLER_API const KMatDef& getMaterialDefinition() const { return m_matDef; }
			
			// @return Body definition this body was created with
			KRAWLER_API const KBodyDef& getBodyDefinition() const { return m_bodyDef; }

			// @param new position
			KRAWLER_API void setPosition(const Vec2f& position) const;

			// @param Rotation in radians,
			KRAWLER_API void setRotiation(float rotation) const;

			// @param Is the body active in simulations
			KRAWLER_API void setActivity(bool bIsActive) const;
			
			/// @return the linear velocity of the center of mass.
			KRAWLER_API Vec2f getLinearVelocity() const;

			/// @param v the new linear velocity of the center of mass.
			KRAWLER_API void setLinearVelocity(const Vec2f& velocity);

			/// @return the angular velocity in radians/second.
			KRAWLER_API float getAngularVelocity() const;

			/// @param omega the new angular velocity in radians/second.
			KRAWLER_API void setAngularVelocity(float velocity);

			/// @param force the world force vector, usually in Newtons (N).
			/// @param point the world position of the point of application.
			/// @param wake also wake up the body
			KRAWLER_API void applyForce(const Vec2f& force, const Vec2f& point, bool wake = true);

			/// @param force the world force vector, usually in Newtons (N).
			/// @param wake also wake up the body
			KRAWLER_API void applyForceToCentre(const Vec2f& force, bool wake = true);

			/// @param torque about the z-axis (out of the screen), usually in N-m.
			/// @param wake also wake up the body
			KRAWLER_API void applyTorque(float torque, bool wake = true);

			/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
			/// @param point the world position of the point of application.
			/// @param wake also wake up the body
			KRAWLER_API void applyLinearImpulse(const Vec2f& impulse, const Vec2f& point, bool wake = true);
			
			/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
			/// @param wake also wake up the body
			KRAWLER_API void applyLinearImpulseToCenter(const Vec2f& impulse, bool wake = true);
			
			/// @return the mass, usually in kilograms (kg).
			KRAWLER_API void getMass() const;

			/// Get the gravity scale of the body.
			KRAWLER_API float getGravityScale() const;

			/// Set the gravity scale of the body.
			KRAWLER_API void setGravityScale(float scale = 1.0f);


		private:
			KBodyDef m_bodyDef;
			KMatDef m_matDef;

			b2PolygonShape m_polygonShape;
			
			Vec2f m_bounds;
			Vec2f m_halfBounds;

			b2Body* m_pB2Body = nullptr;
		};
	}
}

#endif