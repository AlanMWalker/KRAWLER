#pragma once

#include "Krawler.h"
#include "box2d/box2d.h"

// Forward Declerations
class b2World;

namespace Krawler
{
	namespace Physics
	{

		class KPhysicsWorld2D
		{
		public:

			KRAWLER_API KPhysicsWorld2D();
			KRAWLER_API ~KPhysicsWorld2D();

			KRAWLER_API Krawler::KInitStatus initialiseWorld();
			KRAWLER_API void cleanupWorld();

			KRAWLER_API	void stepWorld(float physicsStep);
			KRAWLER_API void setGravity(const Krawler::Vec2f& g);
			KRAWLER_API Krawler::Vec2f getGravity() const;

			// BOX2D INTERACTION FUNCTIONALITY
			b2Body* addNewBody(const b2BodyDef& def);
			void removeBody(b2Body* const pBody);

			KRAWLER_API void rayCast(const Vec2f& start, const Vec2f& end);

			KRAWLER_API void setPPM(float ppm);

			KRAWLER_API float getPPM() const { return m_ppm; }

		private:

			struct RaycastCB : public b2RayCastCallback
			{
				virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
					const b2Vec2& normal, float fraction) override
				{
					Krawler::KPrintf(L"Found intersection on raycast with fraction of %f \n", fraction);
					return 0.0f;
				}

			};

			b2World* m_pBox2DWorld = nullptr;
			Krawler::Vec2f m_gravity = Krawler::Vec2f(0, 9.81f);
			int32 m_velocityIterations = 8;
			int32 m_positionIterations = 3;

			float m_ppm = 1.0f;
		};
	}
}
