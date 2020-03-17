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

			KCBody(KEntity& entity, const Vec2f& bounds);
			KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef);
			KCBody(KEntity& entity, const Vec2f& bounds, const KBodyDef& bodyDef, const KMatDef& matDef);

			virtual ~KCBody() = default;

			virtual KInitStatus init() override;
			virtual void tick() override;

			const KMatDef& getMaterialDefinition() const { return m_matDef; }
			const KBodyDef& getBodyDefinition() const { return m_bodyDef; }

			void setMaterialDefinition(const KMatDef& matDef) { /* Stub for now */ };
			void setBodyDefinition(const KBodyDef& bodyDef) { /* Stub for now */ };

		private:
			KBodyDef m_bodyDef;
			KMatDef m_matDef;
			b2Body* m_pB2Body = nullptr;
			b2PolygonShape m_polygonShape;
			Vec2f m_bounds;
			Vec2f m_halfBounds;
		};
	}
}

#endif