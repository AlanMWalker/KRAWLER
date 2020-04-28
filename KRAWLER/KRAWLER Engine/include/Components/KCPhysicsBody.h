#ifndef KCPHYSICS_BODY_H
#define KCPHYSICS_BODY_H

#include "Krawler.h"
#include "KComponent.h"
#include "KEntity.h"
#include "KCColliderBase.h"

namespace Krawler
{
	namespace Components
	{
		struct KPhysicsBodyProperties
		{
			KRAWLER_API KPhysicsBodyProperties();
			KRAWLER_API KPhysicsBodyProperties(float mass, float staticFriction, float dynamicFriction, float restitution);

			KRAWLER_API void setMass(float mass);
			KRAWLER_API void computeMass(float density, KCColliderType colliderType);

			float mass;
			float invMass;

			float staticFriction;
			float dynamicFriction;

			float restitution;
		};

		class __declspec(deprecated("KPhysicsBody is deprecated, consider using KKinematicBody or KStaticBody instead.")) KCPhysicsBody : public KComponentBase
		{
		public:

			KRAWLER_API KCPhysicsBody(KEntity* pEntity, KPhysicsBodyProperties properties = KPhysicsBodyProperties());
			KRAWLER_API ~KCPhysicsBody() = default;

			KRAWLER_API virtual KInitStatus init() override;
			KRAWLER_API virtual void fixedTick() override;

			KRAWLER_API KPhysicsBodyProperties* getPhysicsBodyProperties() { return &m_properties; }

			KRAWLER_API void applyForce(const Vec2f& force);
			KRAWLER_API void setVelocity(const Vec2f& vel) { m_velocity = vel; }

			KRAWLER_API const Vec2f& getForce() const { return m_force; }
			KRAWLER_API const Vec2f& getVelocity() const { return m_velocity; }

			KRAWLER_API void applyImpulse(const Vec2f& impulse)
			{
				m_velocity += m_properties.invMass * impulse;
			}

			KRAWLER_API bool isBodyStatic() const
			{
				return m_bIsStatic;
			}

			KRAWLER_API void setStaticState(bool bIsStatic)
			{
				m_bIsStatic = bIsStatic;
			}

		private:

			bool m_bIsStatic = false;
			KPhysicsBodyProperties m_properties;

			Vec2f m_velocity;
			Vec2f m_force;
			Vec2f m_prevPosition;
		};
	}
}
#endif
