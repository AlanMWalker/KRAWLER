#include "Maths/KMathVector.h"

float CrossProduct(const sf::Vector2<float>& Vector1, const sf::Vector2<float>& Vector2)
{
	return (Vector1.x * Vector2.y - Vector1.y * Vector2.x);
}

sf::Vector2<float> CrossProduct(const sf::Vector2<float>& Vector1, float Scalar)
{
	return sf::Vector2<float>(Scalar * Vector1.y, -Scalar * Vector1.x);
}

sf::Vector2<float> CrossProduct(float Scalar, const sf::Vector2<float>& Vector1)
{
	return sf::Vector2<float>(-Scalar* Vector1.y, Scalar * Vector1.x);
}