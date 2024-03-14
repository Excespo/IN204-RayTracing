//
// Created by 18490 on 2024/3/13.
//

#ifndef RAY_TRACING_VECTOR_H
#define RAY_TRACING_VECTOR_H

#include <cmath>
#include <iostream>
#include <utility>

// TODO NAMESPACE
class Vector3d {
public: // TODO ALL PUBLIC?
//    Vector3d() = default;
    Vector3d() : _x(0.), _y(0.), _z(0.) {}
    Vector3d(float x, float y, float z) { _x = x; _y = y; _z = z; }
    Vector3d(Vector3d && other) : _x(std::move(other._x)), _y(std::move(other._y)), _z(std::move(other._z)) {}

    inline float get_x() const { return _x; }
    inline float get_y() const { return _y; }
    inline float get_z() const { return _z; }

    inline const Vector3d& operator+ const { return *this; }
    inline Vector3d operator-() const { return Vector3d(-_x, -_y, -_z); }
    inline float operator[] (int i) const {
        if (i == 0) return _x;
        else if (i == 1) return _y;
        else if (i == 2) return _z;
        else throw std::out_of_range("Index out of range in operator[]");
    }
    inline float& operator[] const (int i)

private:
    float _x, _y, _z;
};

#endif //RAY_TRACING_VECTOR_H
