#ifndef KPHYSICS_SCENE_H
#define KPHYSICS_SCENE_H

#include "Krawler.h"
#include "KPhysicsBody.h"
#include "KPhysicsStructs.h"
#include "KPhysicsCollisions.h"
#include "GameObject\KGameObject.h"

//TODO : Sat Impl https://gamedev.stackexchange.com/questions/25397/obb-vs-obb-collision-detection 

namespace Krawler
{
	namespace Physics
	{
		KRAWLER_API extern bool AABBvsAABB(const sf::FloatRect & rectA, const sf::FloatRect & rectB);

		class KPhysicsScene
		{
		public:

			KRAWLER_API KPhysicsScene();
			KRAWLER_API ~KPhysicsScene() = default;

			KRAWLER_API void step(float delta);
			KRAWLER_API void setGravity(Vec2f gravity) { m_gravity = gravity; }
			KRAWLER_API void setPlayerPointer(KPhysicsBody* p) { m_player = p; }
			KRAWLER_API void setPixelsToMetresScale(float scale);

			KRAWLER_API KPhysicsBody* addBody(KGameObject* object, float mass = 1.f, bool subjectToGravity = true);
			KRAWLER_API void removeFromScene(KPhysicsBody* pBody); // (TODO : REQUIRES TESTING) will remove a body from the scene for the duration of the game

			KRAWLER_API void cleanupScene(); // deinit function for PhysicsScene

			//Query
			KRAWLER_API bool doesAABBIntersect(const sf::FloatRect& aabb);
			KRAWLER_API float getPixelsToMetresScale() const { return m_pixelsToMetres; }
			KRAWLER_API int32 getPhysicsBodyCount() const { return KCAST(signed, m_bodies.size()); }
		private:

			void resolveCollision(const KCollisionData&);
			void correctPosition(const KCollisionData&);
			void impulseResolution(KPhysicsBody& bodyA, KPhysicsBody& bodyB, Vec2f collisionNormal, float dp);

			void generateCollisionPairs();
			void handleCollisionPairs();

			const float RESTITUTION = 0.3f;

			bool(*m_collisionFunctions[2][2])(KCollisionData&) = {
				{AABBvsAABBCollision, AABBvsCircleCollision},
				{CircleVsAABBCollision, CirclevsCircleCollision},
			};

			std::vector<KPhysicsBody*> m_bodies;
			std::vector<std::pair<KPhysicsBody*, KPhysicsBody*>> m_uniquePairs;
			KPhysicsBody* m_player = nullptr;

			Vec2f m_gravity;
			float m_pixelsToMetres = 1.f;


		};
	}
}
#endif 
