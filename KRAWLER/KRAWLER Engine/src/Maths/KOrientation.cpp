#include <Maths\KOrientation.h>

using namespace	Krawler;

KOrientation::KOrientation()
{
}

KOrientation::KOrientation(float angleInDeg)
{
	setOrientation(angleInDeg);
}

KOrientation::KOrientation(float mat00, float mat01, float mat10, float mat11)
	: m00(mat00), m01(mat01), m10(mat10), m11(mat11)
{

}

void Krawler::KOrientation::setOrientation(float angleInDeg)
{
	//convert user passed angle into radians
	const float m_theta = Maths::Radians(angleInDeg);

	//Fill out rotation matrix
	m00 = cosf(m_theta); m01 = -sinf(m_theta);
	m10 = sinf(m_theta); m11 = cosf(m_theta);
}

KOrientation KOrientation::getAbsoluteOrientation() const
{
	return KOrientation
	(
		fabs(m00), fabs(m01),
		fabs(m10), fabs(m11)
	);
}

Vec2f KOrientation::getXAxis() const
{
	return Vec2f(m00, m10);
}

Vec2f KOrientation::getYAxis() const
{
	return Vec2f(m01, m11);
}

KOrientation KOrientation::getTransposedOrientation() const
{
	return KOrientation
	(
		m00, m10,
		m01, m11
	);
}

Vec2f KOrientation::operator*(const Vec2f & rhs) const
{
	return Vec2f
	(
		m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y
	);
}

KRAWLER_API KOrientation Krawler::KOrientation::operator*(const KOrientation & rhs) const
{
	// [00 01]  [00 01]
	// [10 11]  [10 11]
	return KOrientation
	(
		m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
		m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
		m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
		m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
	);
}


