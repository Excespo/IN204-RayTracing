//
// Created by LUO Yijie on 2024/2/18.
//

#ifndef RAY_TRACING_GEOMETRY_H
#define RAY_TRACING_GEOMETRY_H

#include <utility>

#include "common.h"
#include "hittable.h"
#include "material.h"

class Sphere: public Hittable {
public:
    Sphere(const Point3d& center, double radius, std::shared_ptr<Material> material)
        : center(center), radius(fmax(radius,0)), material(std::move(material)) {}

    bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const override {
        Vector3d v = center - ray.origin();
        auto a = ray.direction().squared_length();
        auto half_b = dot(ray.direction(), v);
        auto c = v.squared_length() - radius * radius;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrt_dis = std::sqrt(discriminant);
        auto root = (half_b - sqrt_dis) / a;
        if (!t_ray.contains(root)) {
            root = (half_b + sqrt_dis) / a;
            if (!t_ray.contains(root)) return false;
        }

        stat.t = root;
        stat.hit_point = ray.at(stat.t);
        Vector3d outward_normal = (stat.hit_point - center) / radius;
        stat.set_face_normal(ray, outward_normal);
        stat.material = material;
        return true;
    }
private:
    Point3d center;
    double radius;
    std::shared_ptr<Material> material;
};

class Triangle: public Hittable {
    // TODO
};

#endif //RAY_TRACING_GEOMETRY_H
