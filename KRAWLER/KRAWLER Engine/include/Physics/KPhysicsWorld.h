#ifndef KPHYSICS_WORLD_H 
#define KPHYSICS_WORLD_H

#include "Krawler.h"	
#include "KEntity.h"

#include "Utilities\KQuadtree.h"
#include "Components\KCPhysicsBody.h"
#include "Components\KCColliderBase.h"

#include <vector>
#include <functional>

namespace Krawler
{
	namespace Physics
	{

		//defaults: 
		//gravity (0.0f, 9.81) 
		// pixelsToMetre 1:1 ratio (1)
		//correction
		//correction threshold 

		struct KPhysicsWorldProperties
		{
			KRAWLER_API KPhysicsWorldProperties()
				: gravity(0.0f, 9.81f), pixelsToMetres(1.0f), correctionPercentage(0.8f), correctionThreshold(0.2f)
			{

			}

			Vec2f gravity;
			float pixelsToMetres;
			float correctionPercentage;
			float correctionThreshold;
		};

		class KPhysicsWorld
		{
		public:

			KRAWLER_API KPhysicsWorld(KPhysicsWorldProperties properties = KPhysicsWorldProperties());
			KRAWLER_API ~KPhysicsWorld() = default;

			KRAWLER_API const KPhysicsWorldProperties& getPhysicsWorldProperties() const { return m_properties; }

			KRAWLER_API void fixedTick();

			KRAWLER_API void setPhysicsWorldProperties(const KPhysicsWorldProperties& properties) { m_properties = properties; }

			KRAWLER_API int32 getNumberOfPhysicsBodies() const { return m_physicsEntities.size(); }

			void setQuadtree(KQuadtree* pQtree);

		private:
			void resolveCollisions(const KCollisionDetectionData& collData);
			void correctPosition(const KCollisionDetectionData& collData);
			void addToCollisionsList(const KCollisionDetectionData& collData);
			void assembleEntityList();

			std::vector<KEntity*> m_physicsEntities; // entities with physics bodies attached
			std::vector<KCollisionDetectionData> m_collisionsToCheck;

			KPhysicsWorldProperties m_properties;
			KQuadtree* m_pQtree;

			std::function<void(const KCollisionDetectionData& collData)> m_collisionCallback = [this](const KCollisionDetectionData& collData)
			{
				addToCollisionsList(collData);
			};
		};
	}
}

#endif
