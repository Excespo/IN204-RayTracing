//
// Created by LUO Yijie on 2024/2/13.
//

#ifndef RAY_TRACING_VECTOR_H
#define RAY_TRACING_VECTOR_H

#include <cmath>
#include <iostream>
#include "utils.h"

// TODO NAMESPACE
class Vector3d {
public: // TODO ALL PUBLIC?
//    Vector3d() = default;
    Vector3d() : e{0., 0., 0.} {}
    Vector3d(double x, double y, double z) : e{x, y, z} {}
    // TODO move constructor?

    [[nodiscard]] double get_x() const { return e[0]; }
    [[nodiscard]] double get_y() const { return e[1]; }
    [[nodiscard]] double get_z() const { return e[2]; }
    void set_x(double val) { e[0] = val; }
    void set_y(double val) { e[1] = val; }
    void set_z(double val) { e[2] = val; }

    Vector3d operator-() const { return {-e[0], -e[1], -e[2]}; }
    double operator[] (int i) const { return e[i]; }
    double& operator[] (int i) { return e[i]; }

    Vector3d& operator+=(const Vector3d &other) {
        e[0] += other.get_x();
        e[1] += other.get_y();
        e[2] += other.get_z();
        return *this;
    }
    Vector3d& operator-=(const Vector3d &other) {
        return *this += (-other);
    }
    Vector3d& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }
    Vector3d& operator/=(const double t) {
        return *this *= (1/t);
    }

    [[nodiscard]] double squared_length() const { return (e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    [[nodiscard]] double length() const { return std::sqrt(squared_length()); }

    [[nodiscard]] bool near_zero() const {
        auto t = 1e-8; // threshold
        return (std::fabs(e[0]) < t) && (std::fabs(e[1]) < t) && (std::fabs(e[2]) < t);
    }

    static Vector3d random() {
        return {random_double(), random_double(), random_double()};
    }

    static Vector3d random(double min, double max) {
        return {random_double(min, max), random_double(min, max), random_double(min, max)};
    }

private:
    double e[3];
};

using Point3d = Vector3d;

inline std::ostream& operator<< (std::ostream& os, const Vector3d& vec) {
    os << "[Vector3d ]: (" << vec.get_x() << "," << vec.get_y() << "," << vec.get_z() << ")";
    return os;
}

inline std::ostream& operator<< (std::ostream& os,  Vector3d& vec) {
    os << "[Vector3d ]: (" << vec.get_x() << "," << vec.get_y() << "," << vec.get_z() << ")";
    return os;
}

inline std::istream& operator>>(std::istream &is, Vector3d &vec) {
    double x, y, z;
    is >> x >> y >> z;
    vec.set_x(x);
    vec.set_y(y);
    vec.set_z(z);
    return is;
}

inline Vector3d operator+(const Vector3d& v1, const Vector3d& v2) {
    return {v1.get_x() + v2.get_x(), v1.get_y() + v2.get_y(), v1.get_z() + v2.get_z()};
}

inline Vector3d operator-(const Vector3d& v1, const Vector3d& v2) {
    return {v1.get_x() - v2.get_x(), v1.get_y() - v2.get_y(), v1.get_z() - v2.get_z()};
}

inline Vector3d operator*(const Vector3d& v1, const Vector3d& v2) {
    return {v1.get_x() * v2.get_x(), v1.get_y() * v2.get_y(), v1.get_z() * v2.get_z()};
}

inline Vector3d operator*(double t, const Vector3d& v) {
    return {t*v.get_x(), t*v.get_y(), t*v.get_z()};
}

inline Vector3d operator*(const Vector3d& v, double t) {
    return t * v;
}

inline Vector3d operator/(const Vector3d& v, double t) {
    return (1/t) * v;
}

inline double dot(const Vector3d& v1, const Vector3d& v2) {
    return v1.get_x() * v2.get_x()
           + v1.get_y() * v2.get_y()
           + v1.get_z() * v2.get_z();
}

inline Vector3d cross(const Vector3d& v1, const Vector3d& v2) {
    return {v1.get_y() * v2.get_z() - v1.get_z() * v2.get_y(),
            v1.get_z() * v2.get_x() - v1.get_x() * v2.get_z(),
            v1.get_x() * v2.get_y() - v1.get_y() * v2.get_x()};
}

inline Vector3d unit_vector(const Vector3d& v) {
    return v / v.length();
}

inline Vector3d random_in_unit_disk() {
    while(true) {
        auto v = Vector3d(random_double(-1, 1), random_double(-1, 1), 0);
        if (v.squared_length() < 1)
            return v;
    }
}

inline Vector3d random_in_unit_sphere() {
    while(true) {
        auto v = Vector3d::random(-1,1);
        if (v.squared_length() < 1)
            return v;
    }
}

inline Vector3d random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline Vector3d random_on_hemisphere(const Vector3d& normal) {
    Vector3d on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

inline Vector3d reflect(const Vector3d& vec_in, const Vector3d& normal) {
    return vec_in - 2 * dot(vec_in, normal) * normal;
}

inline Vector3d refract(const Vector3d& vec, const Vector3d& normal, double eta_i_over_t) {
    auto cos_theta = fmin(dot(-vec, normal), 1.0);
    Vector3d ray_out_perp = eta_i_over_t * (vec + cos_theta*normal);
    Vector3d ray_out_para = -std::sqrt(std::fabs(1.0 - ray_out_perp.squared_length())) * normal;
    return ray_out_perp + ray_out_para;
}

#endif //RAY_TRACING_VECTOR_H
