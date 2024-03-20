//
// Created by LUO Yijie on 2024/3/20.
//

#ifndef RAY_TRACING_AABB_H
#define RAY_TRACING_AABB_H

// AABB stands for Axis Align Bounding Box

#include "utils.h"
#include "interval.h"
#include "vector.h"
#include "ray.h"

class AABB {
public:
    Interval x, y, z;

    AABB() = default;
    AABB(const Interval& x, const Interval& y, const Interval& z): x(x), y(y), z(z) {}

    AABB(const Point3d& a, const Point3d& b) {
        x = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
        y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
        z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
    }

    AABB(const AABB& box1, const AABB& box2) {
        x = Interval(box1.x, box2.x);
        y = Interval(box1.y, box2.y);
        z = Interval(box1.z, box2.z);
    }

    const Interval& axis (int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    [[deprecated]] bool hit_v1 (const Ray& ray, Interval t_ray) const {
        for (int a = 0; a < 3; a++) {
            auto t0 = fmin(
                    (axis(a).get_min() - ray.origin()[a]) / ray.direction()[a],
                    (axis(a).get_max() - ray.origin()[a]) / ray.direction()[a]);
            auto t1 = fmax(
                    (axis(a).get_min() - ray.origin()[a]) / ray.direction()[a],
                    (axis(a).get_max() - ray.origin()[a]) / ray.direction()[a]);
            t_ray.set_min(fmax(t0, t_ray.get_min()));
            t_ray.set_max(fmin(t1, t_ray.get_max()));
            if (t_ray.get_max() <= t_ray.get_min()) return false;
        }
        return true;
    }

    bool hit (const Ray& ray, Interval t_ray) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1 / ray.direction()[a];
            auto orig = ray.origin()[a];
            auto t0 = (axis(a).get_min() - orig) * invD;
            auto t1 = (axis(a).get_max() - orig) * invD;
            if (invD < 0)
                std::swap(t0, t1);
            if (t0 > t_ray.get_min())
                t_ray.set_min(t0);
            if (t1 < t_ray.get_max())
                t_ray.set_max(t1);
            if (t_ray.get_min() >= t_ray.get_max())
                return false;
        }
        return true;
    }

    int longest_axis() const {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const AABB empty, universe;
};

const AABB AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);


#endif //RAY_TRACING_AABB_H
