#ifndef KMATH_VECTOR_H
#define KMATH_VECTOR_H

#include <SFML\System\Vector2.hpp>
#include <ostream>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

/**
- SFML Vectors functionality extensions
**/


//using namespace sf;
///Extension functions for SFML vectos 

///Function to normalise vector
///Param: ref Vector2
///Return: Vector2
template<typename T>
__forceinline sf::Vector2<T> Normalise(const sf::Vector2<T>& Vector);

///Get the dot product between two vectors
///Param: ref Vector2
///Return: type of vector(float, int etc..)
template<typename T>
__forceinline T DotProduct(const sf::Vector2<T>& Vector1, const sf::Vector2<T>& Vector2);

///Get the length of a vector
///Param: ref Vector2
///Return: type of vector 
template<typename T>
__forceinline T GetLength(const sf::Vector2<T>& Vector);

///Get the squared length of a vector 
///Param: ref Vector2
///Return: Type of vector
template<typename T>
__forceinline T GetSquareLength(const sf::Vector2<T>& Vector);

///Set a vector to 0, 0
///Param: ref Vector2
///Return: Vector2
template<typename T>
__forceinline void Zero(sf::Vector2<T>& Vector);

///Get angle between two vectors (in radians)
///Param: ref Vector2
///Return: Type of vector
template<typename T>
__forceinline T GetAngleBetween(const sf::Vector2<T>& VectorA, const sf::Vector2<T>& VectorB);

template<typename T>
__forceinline float GetAngle(const sf::Vector2<T>&Vector);

///Rotate a vector by angle theta (in radians)
///Param: ref Vector2
///Param: float Angle (radians)
///Return: Type of vector
template<typename T>
__forceinline sf::Vector2<T> RotateVector(const sf::Vector2<T>& Vector, float Angle);

///Output stream operator for Vector2
template<typename T>
__forceinline std::ostream& operator<< (std::ostream& OStream, const sf::Vector2<T>& Vector);

///Input stream operator for Vector2
template<typename T>
__forceinline std::istream& operator >> (std::istream& IStream, sf::Vector2<T>& Vector);

#include "KMathVector.inl"

#endif
