#ifndef SKY_VECTOR_HPP
#define SKY_VECTOR_HPP

#include <cmath>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace sky {
template<typename T>
inline T dot(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T>
inline T dot(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename T>
inline sf::Vector3<T> cross(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs) {
    return { 0, 0, lhs.x * rhs.y - lhs.y * rhs.x };
}

template<typename T>
inline sf::Vector3<T> cross(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs) {
    return { lhs.y * rhs.z - lhs.z * rhs.y,
             lhs.z * rhs.x - lhs.x * rhs.z,
             lhs.x * rhs.y - lhs.y * rhs.x };
}

template<typename Vector>
inline auto squaredLength(const Vector& vector) -> decltype(dot(vector, vector)) {
    return dot(vector, vector);
}

template<typename Vector>
inline auto length(const Vector& vector) -> decltype(std::sqrt(squaredLength(vector))) {
    return std::sqrt(squaredLength(vector));
}
} // sky

#endif // SKY_VECTOR_HPP
