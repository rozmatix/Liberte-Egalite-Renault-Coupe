#pragma once
#include <cmath> // std::sqrt

class Vector2
{
public:
    float x;
    float y;

    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
    Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
    Vector2 operator*(float scalar) const { return { x * scalar, y * scalar }; }

    // metoda za dolžino vektorja
    float Length() const
    {
        return std::sqrt(x * x + y * y);
    }

    // metoda za enotski vektor (normalizacija)
    Vector2 Normalized() const
    {
        float len = Length();
        if (len == 0) return { 0, 0 }; // varno za nièelni vektor
        return { x / len, y / len };
    }
};
