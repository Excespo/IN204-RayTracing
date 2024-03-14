//
// Created by 18490 on 2024/3/13.
//

#ifndef RAY_TRACING_RAY_H
#define RAY_TRACING_RAY_H
#include "vector.h"

class Ray {
private:
    Vector3d _A;
    Vector3d _B;
public:
    ray() {}
    ray(const Vector3d& A, const Vector3d& B) { _A = A; _B = B; }
    Vector3d origin() const { return A; }
    Vector3d direction() const { return B; }
    Vector3d parameterized_point_at(float t) { return A + t * B; }

};

#endif //RAY_TRACING_RAY_H
