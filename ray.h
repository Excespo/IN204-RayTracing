//
// Created by LUO Yijie on 2024/2/13.
//

#ifndef RAY_TRACING_RAY_H
#define RAY_TRACING_RAY_H

#include "utils.h"

class Ray {
public:
    Ray() = default;
    Ray(const Point3d& origin, const Vector3d& direction)
        : orig(origin), dir(direction), tm(0) {}
    Ray(const Point3d& origin, const Vector3d& direction, double time)
        : orig(origin), dir(direction), tm(time) {}

    [[nodiscard]] Point3d origin() const  { return orig; }
    [[nodiscard]] Vector3d direction() const { return dir; }
    double time() const { return tm; }

    [[nodiscard]] Point3d at(double t) const {
        return orig + t*dir;
    }

private:
    Point3d orig;
    Vector3d dir;
    double tm;
};

#endif //RAY_TRACING_RAY_H
