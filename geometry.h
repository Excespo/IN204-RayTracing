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
            : center(center), radius(fmax(radius,0)), material(std::move(material))
    {
        auto radvec = Vector3d(radius, radius, radius);
        bbox = AABB(center - radvec, center + radvec);
    }

    AABB bounding_box() const override { return bbox; }

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
    AABB bbox;
};

class Quadrilateral: public Hittable {
public:
    Quadrilateral(const Point3d& Q, const Vector3d& u, const Vector3d& v, std::shared_ptr<Material> material)
        : Q(Q), u(u), v(v), material(material) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        auto bbox_diagonal1 = AABB(Q, Q+u+v);
        auto bbox_diagonal2 = AABB(Q+u, Q+v);
        bbox = AABB(bbox_diagonal1, bbox_diagonal2);
    }

    AABB bounding_box() const override { return bbox; }

    bool hit (const Ray& ray, Interval t_ray, HitStatus& stat) const override {
        auto denom = dot(normal, ray.direction());

        // No hit if the ray is parallel with the plane
        if (fabs(denom) < 1e-8) return false;

        // False if hit time outside the ray interval
        auto t = (D - dot(normal, ray.origin())) / denom;
        if (!t_ray.contains(t)) return false;

        // Determine hit point lies within the planar shape using its plane coordinates
        auto intersection = ray.at(t);
        Vector3d planar_hitpoint_vec = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpoint_vec, v));
        auto beta = dot(w, cross(u, planar_hitpoint_vec));

        if (!is_interior(alpha, beta, stat))
            return false;

        stat.t = t;
        stat.hit_point = intersection;
        stat.material = material;
        stat.set_face_normal(ray, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, HitStatus& stat) const {
        Interval unit_interval = Interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        stat.u = a;
        stat.v = b;
        return true;
    }

private:
    Point3d Q; // left bottom corner of quad
    Vector3d u; // vector leading to one side
    Vector3d v; // vector leading to another side
    Vector3d w;
    std::shared_ptr<Material> material;
    AABB bbox;
    Vector3d normal;
    double D;
};

class Triangle: public Hittable {
    // TODO
};

#endif //RAY_TRACING_GEOMETRY_H
