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
		};

		struct KCollisionData
		{
			KPhysicsBody* bodyA;
			KPhysicsBody* bodyB;
			float penetration = 0.0f;
			Krawler::Vec2f collisionNormal;
		};
	}
}
#endif
