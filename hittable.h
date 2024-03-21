//
// Created by LUO Yijie on 2024/2/18.
//

#ifndef RAY_TRACING_HITTABLE_H
#define RAY_TRACING_HITTABLE_H

#include <iostream>
#include <memory>
#include <algorithm>
#include "common.h"

class Material;

class HitStatus {
public:
    Point3d hit_point;
    Vector3d normal;
    std::shared_ptr<Material> material;
    double t;
    double u, v; // quadrilateral
    bool front_face;

    void set_face_normal(const Ray& ray, const Vector3d& outward_normal) {
        // Design here is normal vector always points against the ray's direction
        // Should set outward_normal vector to length 1
        front_face = dot(ray.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const = 0;
    virtual AABB bounding_box() const = 0;
};

class HittableList: public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() = default;
    explicit HittableList(std::shared_ptr<Hittable> obj) { add(std::move(obj)); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<Hittable> obj) {
        objects.push_back(std::move(obj));
        bbox = AABB(bbox, obj->bounding_box());
    }

    bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const override {
        HitStatus temp_stat;
        auto is_hit = false;
        auto closest_so_far = t_ray.get_max();

        for (const auto& obj : objects) {
            if (obj->hit(ray, Interval(t_ray.get_min(), closest_so_far), temp_stat)) {
                is_hit = true;
                closest_so_far = temp_stat.t;
                stat = temp_stat;
            }
        }

        return is_hit;
    }
    
    AABB bounding_box() const override { return bbox; }

private:
    AABB bbox;
};

class translate: public Hittable {
    // TODO
};

class rotate_y: public Hittable {
    // TODO
};

class BVH_Node: public Hittable {
public:
    explicit BVH_Node(HittableList list) : BVH_Node(list.objects, 0, list.objects.size()) {}
    BVH_Node(std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end){
        // Build the bounding box of the span of source objects.
        bbox = AABB::empty;
        for (size_t object_index=start; object_index < end; object_index++)
            bbox = AABB(bbox, src_objects[object_index]->bounding_box());

        int axis = bbox.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = src_objects[start];
        } else if (object_span == 2) {
            left = src_objects[start];
            right = src_objects[start+1];
        } else {
            std::sort(src_objects.begin() + start, src_objects.begin() + end, comparator);

            auto mid = start + object_span/2;
            left = std::make_shared<BVH_Node>(src_objects, start, mid);
            right = std::make_shared<BVH_Node>(src_objects, mid, end);
        }
    }

    bool hit(const Ray& ray, Interval t_ray, HitStatus& stat) const override {
        if (!bbox.hit(ray, t_ray)) return false;
        bool hit_left = left->hit(ray, t_ray, stat);
        bool hit_right = right->hit(ray, Interval(t_ray.get_min(), hit_left ? stat.t : t_ray.get_max()), stat);
        return hit_left || hit_right;
    }

    AABB bounding_box() const override { return bbox; }

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB bbox;

    static bool box_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int idx_axis) {
        return a->bounding_box().axis(idx_axis).get_min() < b->bounding_box().axis(idx_axis).get_min();
    }

    static bool box_x_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
        return box_compare(a, b, 2);
    }
};

#endif //RAY_TRACING_HITTABLE_H
