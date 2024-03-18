//
// Created by 18490 on 2024/3/18.
//

#ifndef RAY_TRACING_HITTABLE_H
#define RAY_TRACING_HITTABLE_H

#include <iostream>

#include "common.h"

class Material;

class HitStatus {
public:
    Point3d hit_point;
    Vector3d normal;
    std::shared_ptr<Material> material;
    double t;
    bool front_face;

    void set_face_normal(const Ray& ray, const Vector3d& outward_normal) {
        // Should set outward_normal vector to length 1
        front_face = dot(ray.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const = 0;
};

class HittableList: public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> hittable_objs;

    HittableList() = default;
    explicit HittableList(std::shared_ptr<Hittable> obj) { add(std::move(obj)); }

    void clear() { hittable_objs.clear(); }

    void add(std::shared_ptr<Hittable> obj) {
        hittable_objs.push_back(std::move(obj));
    }

    bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const override {
        HitStatus temp_stat;
        auto is_hit = false;
        auto closest_so_far = t_ray.get_max();

        for (const auto& obj : hittable_objs) {
            if (obj->hit(ray, Interval(t_ray.get_min(), closest_so_far), temp_stat)) {
                is_hit = true;
                closest_so_far = temp_stat.t;
                stat = temp_stat;
            }
        }

        return is_hit;
    }

};

#endif //RAY_TRACING_HITTABLE_H
