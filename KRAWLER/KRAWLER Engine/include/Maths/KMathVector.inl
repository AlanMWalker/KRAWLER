#include "KMathVector.h"

/////////////////////////////////////

template<typename T>
__forceinline sf::Vector2<T> Normalise(const sf::Vector2<T>& Vector)
{
	T Length = static_cast<T> (GetLength(Vector));

	if (Length != 0)
	{
		return(Vector * static_cast<T>(1.f / Length));
	}
	return Vector;

}

template<typename T>
__forceinline T DotProduct(const sf::Vector2<T>& Vector1, const sf::Vector2<T>& Vector2)
{
	return{ static_cast<T> ((Vector1.x * Vector2.x) + (Vector1.y * Vector2.y)) };
}

__forceinline KRAWLER_API float CrossProduct(const sf::Vector2<float>& Vector1, const sf::Vector2<float>& Vector2)
{
	return (Vector1.x * Vector2.y - Vector1.y * Vector2.x);
}

inline KRAWLER_API sf::Vector2<float> CrossProduct(const sf::Vector2<float>& Vector1, float Scalar)
{
	return sf::Vector2<float>(Scalar * Vector1.y, -Scalar * Vector1.x);
}

inline KRAWLER_API sf::Vector2<float> CrossProduct(float Scalar, const sf::Vector2<float>& Vector1)
{
	return sf::Vector2<float>(-Scalar* Vector1.y, Scalar * Vector1.x);
}

template<typename T>
__forceinline T GetLength(const sf::Vector2<T>& Vector)
{
	return{ static_cast<T> (sqrt(GetSquareLength(Vector))) };
}

template<typename T>
__forceinline T GetSquareLength(const sf::Vector2<T>& Vector)
{
	return{ (Vector.x * Vector.x) + (Vector.y * Vector.y) };
}

template<typename T>
__forceinline void Zero(sf::Vector2<T>& Vector)
{
	Vector.x = 0;
	Vector.y = 0;
}

template<typename T>
__forceinline T GetAngleBetween(const sf::Vector2<T>& VectorA, const sf::Vector2<T>& VectorB)
{
	T DP{ DotProduct(VectorA, VectorB) };

	return{ static_cast<T> (acos(DP / (GetLength(VectorA) * GetLength(VectorB)))) };
}

template<typename T>
__forceinline float GetAngle(const sf::Vector2<T>& Vector)
{
	return atan2f((float)Vector.y, (float)Vector.x);
}

template<typename T>
__forceinline sf::Vector2<T> RotateVector(const sf::Vector2<T>& Vector, float Angle)
{
	sf::Vector2<T> Vec;
	Angle = Maths::Radians(Angle);

	Vec.x = static_cast<T> ((Vector.x * cos(Angle)) - (Vector.y * sin(Angle)));
	Vec.y = static_cast<T> ((Vector.x * sin(Angle)) + (Vector.y * cos(Angle)));
	return{ Vec };
}

template<typename T>
__forceinline std::ostream& operator<<(std::ostream & OStream, const sf::Vector2<T>& Vector)
{
	OStream << Vector.x << " : " << Vector.y;
	return(OStream);
}

template<typename T>
__forceinline std::istream& operator >> (std::istream& IStream, sf::Vector2<T>& Vector)
{
	IStream >> Vector.x >> Vector.y;
	return (IStream);
}

/////////////////////////////////////