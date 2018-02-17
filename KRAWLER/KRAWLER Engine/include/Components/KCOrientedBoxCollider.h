#ifndef KCORIENTED_BOX_COLLIDER_H
#define KCORIENTED_BOX_COLLIDER_H

#include <Krawler.h>
#include <Components\KCTransform.h>
#include <Components\KCColliderBase.h>
#include <Maths\KOrientation.h>

namespace Krawler
{
	namespace Components
	{
		constexpr int32 FaceCount = 4;

		class KCOrientedBoxCollider : public KCColliderBase
		{
		public:

			KRAWLER_API KCOrientedBoxCollider(KEntity* pEntity, const Vec2f& size);
			KRAWLER_API ~KCOrientedBoxCollider() = default;

			KRAWLER_API virtual const Rectf& getBoundingBox() override;

			KRAWLER_API virtual void tick() override;
			KRAWLER_API virtual void fixedTick() override;

			KRAWLER_API Vec2f getOrientedFaceNormal(int faceIdx) const;
			KRAWLER_API const Vec2f& getNormal(int faceIdx) const;

			KRAWLER_API Vec2f getVertexWorldPosition(int vertIdx) const;
			KRAWLER_API const Vec2f& getVertexLocalPosition(int vertIdx) const;

			KRAWLER_API Vec2f getSupport(const Vec2f& dir) const;
			KRAWLER_API const KOrientation& getOrientation() const { return m_orientation; }
			KRAWLER_API Vec2f getPosition() const { return m_pTransform->getPosition(); }
		private:

			Vec2f m_size;
			Rectf m_boundingBox;

			KCTransform* m_pTransform;
			KOrientation m_orientation;

			Vec2f m_vertices[FaceCount];

			Vec2f m_normals[FaceCount];/* =
			{
				Vec2f(0.0,-1.0f),
				Vec2f(1.0f, 0.0f),
				Vec2f(0.0f, 1.0f),
				Vec2f(-1.0f, 0.0f)
			};*/

		};
	}
}
#endif 
