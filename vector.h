//
// Created by 18490 on 2024/3/13.
//

#ifndef RAY_TRACING_VECTOR_H
#define RAY_TRACING_VECTOR_H

#include <cmath>
#include <iostream>

// TODO NAMESPACE
class Vector3d {
public: // TODO ALL PUBLIC?
//    Vector3d() = default;
    Vector3d() : e{0., 0., 0.} {}
    Vector3d(double x, double y, double z) : e{x, y, z} {}
    Vector3d(Vector3d && other) { e[0] = other.e[0]; e[1] = other.e[1]; e[2] = other.e[2]; }
    Vector3d(const Vector3d & other) { e[0] = other.e[0]; e[1] = other.e[1]; e[2] = other.e[2]; }

    inline double get_x() const { return e[0]; }
    inline double get_y() const { return e[1]; }
    inline double get_z() const { return e[2]; }
    inline double get_r() const { return get_x(); }
    inline double get_g() const { return get_y(); }
    inline double get_b() const { return get_z(); }

    inline double set_x(double val) const { e[0] = val; }
    inline double set_y(double val) const { e[1] = val; }
    inline double set_z(double val) const { e[2] = val; }
    inline double set_r(double val) const { e[0] = val; }
    inline double set_g(double val) const { e[1] = val; }
    inline double set_b(double val) const { e[2] = val; }

    inline const Vector3d& operator+() const { return *this; }
//    inline Vector3d operator-() const { return Vector3d(-e[0], -e[1], -e[2]); }
    inline Vector3d operator-() const { return {-e[0], -e[1], -e[2]}; }
    inline double operator[] (int i) { return e[i]; }
    inline double& operator[] (int i) { return e[i]; }

    inline Vector3d operator+=(const Vector3d &other) {
//        set_x(get_x() + other.get_x());
//        set_y(get_y() + other.get_y());
//        set_z(get_z() + other.get_z());
        e[0] += other.get_x();
        e[1] += other.get_y();
        e[2] += other.get_z();
        return *this;
    }
    inline Vector3d operator-=(const Vector3d &other) {
        return *this += (-other)
    }
    inline Vector3d operator*=(const Vector3d &other);
    inline Vector3d operator/=(const Vector3d &other);
    inline Vector3d operator*=(const double t) {

    }
    inline Vector3d operator/=(const double t);

    inline double squared_length() const { return (e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    inline double length() const { return std::sqrt(squared_length()); }
    inline void normalize();

private:
    double e[3];
};

using Point3d = Vector3d;

inline std::ostream& operator<<(std::ostream &os, Vector3d &vec) {
    os << "[Vector3d ]: (" << vec.get_x() << "," << vec.get_y() << "," << vec.get_z() << ")";
    return os;
}

inline std::istream& operator<<(std::istream &is, Vector3d &vec) {
    double x, y, z;
    is >> x >> y >> z;
    vec.set_x(x);
    vec.set_y(y);
    vec.set_z(z);
}

inline void Vector3d::normalize() {
    double norm2 = squared_length();
    set_x(1/norm2 * get_x());
    set_y(1/norm2 * get_y());
    set_z(1/norm2 * get_z());
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator/(vec3 v, double t) {
    return vec3(v.e[0]/t, v.e[1]/t, v.e[2]/t);
}

inline vec3 operator*(const vec3 &v, double t) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline double dot(const vec3 &v1, const vec3 &v2) {
    return v1.e[0] * v2.e[0]
           + v1.e[1] * v2.e[1]
           + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
                v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2],
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3& vec3::operator+=(const vec3 &v){
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3 &v){
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3 &v){
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3& vec3::operator/=(const double t) {
    double k = 1.0f/t;

    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

#endif //RAY_TRACING_VECTOR_H
