#ifndef KCPHYSICS_BODY_H
#define KCPHYSICS_BODY_H

#include "Krawler.h"
#include "KComponent.h"
#include "KEntity.h"

namespace Krawler
{
	namespace Components
	{
		enum KColliderType
		{
			AABB,
			Circle
		};

		struct KPhysicsBodyProperties
		{
			KRAWLER_API KPhysicsBodyProperties();

			KRAWLER_API void setMass(float mass);
			KRAWLER_API void computeMass(float density, KColliderType colliderType);

			float mass;
			float invMass;

			float staticFriction;
			float dynamicFriction;

			float restitution;
		};

		class KCPhysicsBody : public KComponentBase
		{
		public:

			KRAWLER_API KCPhysicsBody(KEntity* pEntity, KPhysicsBodyProperties properties = KPhysicsBodyProperties());
			KRAWLER_API ~KCPhysicsBody() = default;

			KRAWLER_API virtual KInitStatus init() override;
			KRAWLER_API virtual void fixedTick() override;

			KRAWLER_API KPhysicsBodyProperties* getPhysicsBodyProperties() { return &m_properties; }

			KRAWLER_API void applyForce(const Vec2f& force);

		private:

			KPhysicsBodyProperties m_properties;

			Vec2f m_velocity;
			Vec2f m_force;
			Vec2f m_prevPosition;
		};
	}
}
#endif
