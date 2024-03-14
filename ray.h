//
// Created by 18490 on 2024/3/13.
//

#ifndef RAY_TRACING_RAY_H
#define RAY_TRACING_RAY_H
#include "vector.h"

class Ray {
public:
    Ray() {}
    Ray(const Point3d& origin, const Vector3d& direction) : orig(origin), dir(direction) {}

    Point3d origin() const  { return orig; }
    Vector3d direction() const { return dir; }

    Point3d at(double t) const {
        return orig + t*dir;
    }

private:
    Point3d orig;
    Vector3d dir;
};

#endif //RAY_TRACING_RAY_H
