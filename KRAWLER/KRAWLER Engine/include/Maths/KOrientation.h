#ifndef KORIENTATION_H
#define KORIENTATION_H

#include <Krawler.h>

namespace Krawler
{
	//2x2 rotation matrix for orientation collision detection
	//m00,m01 = cos(theta), -sin(theta)
	//m10,m11 = sin(theta), cos(theta) 
	// (Transposing of this orientation will provide the inverse
	//	rotation)
	struct KOrientation
	{
		union
		{
			struct
			{
				float m00, m01;
				float m10, m11;
			};

			float m[2][2];
			float v[4];
		};

		KRAWLER_API KOrientation();
		KRAWLER_API KOrientation(float angleInDeg);
		KRAWLER_API KOrientation(float mat00, float mat01, float mat10, float mat11);

		KRAWLER_API void setOrientation(float angleInDeg);
		KRAWLER_API KOrientation getAbsoluteOrientation() const;

		KRAWLER_API Vec2f getXAxis() const;
		KRAWLER_API Vec2f getYAxis() const;

		KRAWLER_API KOrientation getTransposedOrientation() const;
		KRAWLER_API Vec2f operator*(const Vec2f& rhs) const;
		KRAWLER_API KOrientation operator* (const KOrientation& rhs) const;
	};
}

#endif