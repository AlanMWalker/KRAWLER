#ifndef KPHYSICS_STRUCTS_H
#define KPHYSICS_STRUCTS_H

#include "Krawler.h"

//Forward Declerations
namespace Krawler
{
	namespace Physics
	{
		class KPhysicsBody;

		struct MassData
		{
			MassData() :
				mass(0.0f), invMass(1.0f)
			{

			}

			MassData(float inMass)
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

		struct MaterialData
		{
			MaterialData()
				: restitution(1.0f), density(1.0f), gravityScale(1.0f)
			{
				//ctor
			}
			MaterialData(float inDensity, float inRestitution, float inGravityScale)
				: density(inDensity), restitution(inRestitution), gravityScale(inGravityScale)
			{

			}

			float restitution;
			float density;
			float gravityScale;
		};

		struct CollisionData
		{
			KPhysicsBody* bodyA;
			KPhysicsBody* bodyB;
			float penetration = 0.0f;
			Krawler::Vec2f collisionNormal;
		};
	}
}
#endif
