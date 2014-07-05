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

template<typename Vector>
inline auto lerp(const Vector& from, const Vector& to, double t) -> decltype(from * t + (to * (1 - t))) {
    return from * t + (to * (1 - t));
}

template<typename T>
inline auto direction(const sf::Vector2<T>& from, const sf::Vector2<T>& to) -> decltype(std::atan2(to.y - from.y, to.x - from.x)) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

template<typename Vector>
inline auto angle(const Vector& u, const Vector& v) -> decltype(std::acos(dot(u, v) / (length(u) * length(v)))) {
    return std::acos(dot(u, v) / (length(u) * length(v)));
}

template<typename Vector>
inline bool isUnit(const Vector& vector) {
    return squaredLength(vector) == 1;
}

template<typename Vector>
inline bool perpendicular(const Vector& lhs, const Vector& rhs) {
    return dot(lhs, rhs) == 0;
}

template<typename Vector>
inline Vector normalized(Vector vector) {
    auto&& len = length(vector);

    if(len != 0 && len != 1) {
        vector /= len;
    }

    return vector;
}

template<typename Vector>
inline auto distance(const Vector& from, const Vector& to) -> decltype(length(from - to)) {
    return length(from - to);
}

template<typename Vector>
inline auto squaredDistance(const Vector& from, const Vector& to) -> decltype(squaredLength(from - to)) {
    return squaredLength(from - to);
}
} // sky

#endif // SKY_VECTOR_HPP
