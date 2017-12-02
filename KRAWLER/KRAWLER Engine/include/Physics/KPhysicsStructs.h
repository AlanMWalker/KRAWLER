#ifndef KPHYSICS_STRUCTS_H
#define KPHYSICS_STRUCTS_H

#include "Krawler.h"

//Forward Declerations
namespace Krawler
{
	namespace Physics
	{
		class KPhysicsBody;

		struct KMassData
		{
			KMassData() :
				mass(0.0f), invMass(1.0f)
			{

			}

			KMassData(float inMass)
			{
				setMass(inMass);
			}

			void setMass(float inMass)
			{
				if (inMass != 0.0f)
				{
					mass = inMass;
					invMass = 1.0f / inMass;
				}
				else
				{
					mass = 0.0f;
					invMass = 1.0f;
				}
			}

			float mass;
			float invMass;
		};

		struct KMaterialData
		{
			KMaterialData()
				: restitution(1.0f), density(1.0f), gravityScale(1.0f)
			{
				//ctor
			}
			KMaterialData(float inDensity, float inRestitution, float inGravityScale)
				: density(inDensity), restitution(inRestitution), gravityScale(inGravityScale)
			{

			}

			float restitution;
			float density;
			float gravityScale;
			float staticFriction = 0.3f;
			float dynamicFriction = 0.15f;
		};

		struct KCollisionData
		{
			KPhysicsBody* bodyA;
			KPhysicsBody* bodyB;
			float penetration = 0.0f;
			Krawler::Vec2f collisionNormal;

			Vec2f contacts[2];
			uint32 contactCount = 0;
		};

		struct KPhysicsStateData
		{
			Vec2f currentVelocity;
			Vec2f lastVelocity;

			Vec2f force;
			KMassData mass;

			float orientation = 0.0f; // Radians
			float angularVelocity = 0.0f;
			float torque = 0.0f;
		};
	}
}
#endif
