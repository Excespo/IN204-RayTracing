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
        if (!t_ray.surrounds(root)) {
            root = (half_b + sqrt_dis) / a;
            if (!t_ray.surrounds(root)) return false;
        }

        stat.t = root;
        stat.hit_point = ray.at(stat.t);
        Vector3d outward_normal = (stat.hit_point - center) / radius;
        stat.set_face_normal(ray, outward_normal);
        get_sphere_uv(outward_normal, stat.u, stat.v);
        stat.material = material;
        return true;
    }
private:
    Point3d center;
    double radius;
    std::shared_ptr<Material> material;
    AABB bbox;

    static void get_sphere_uv(const Point3d& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.get_y());
        auto phi = std::atan2(-p.get_y(), p.get_y()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
};

class Quadrilateral: public Hittable {
public:
    Quadrilateral(const Point3d& Q, const Vector3d& u, const Vector3d& v, std::shared_ptr<Material> material)
        : Q(Q), u(u), v(v), material(std::move(material)) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    void set_bounding_box() {
//    virtual void set_bounding_b.get_x() {
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
    Point3d Q; // left bottom corner of Quadrilateral
    Vector3d u; // vector leading to one side
    Vector3d v; // vector leading to another side
    Vector3d w;
    std::shared_ptr<Material> material;
    AABB bbox;
    Vector3d normal;
    double D;
};

inline std::shared_ptr<HittableList> box(const Point3d& a, const Point3d& b, std::shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = std::make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Point3d(fmin(a.get_x(), b.get_x()), fmin(a.get_y(), b.get_y()), fmin(a.get_z(), b.get_z()));
    auto max = Point3d(fmax(a.get_x(), b.get_x()), fmax(a.get_y(), b.get_y()), fmax(a.get_z(), b.get_z()));

    auto dx = Vector3d(max.get_x() - min.get_x(), 0, 0);
    auto dy = Vector3d(0, max.get_y() - min.get_y(), 0);
    auto dz = Vector3d(0, 0, max.get_z() - min.get_z());

    sides->add(std::make_shared<Quadrilateral>(Point3d(min.get_x(), min.get_y(), max.get_z()),  dx,  dy, mat)); // front
    sides->add(std::make_shared<Quadrilateral>(Point3d(max.get_x(), min.get_y(), max.get_z()), -dz,  dy, mat)); // right
    sides->add(std::make_shared<Quadrilateral>(Point3d(max.get_x(), min.get_y(), min.get_z()), -dx,  dy, mat)); // back
    sides->add(std::make_shared<Quadrilateral>(Point3d(min.get_x(), min.get_y(), min.get_z()),  dz,  dy, mat)); // left
    sides->add(std::make_shared<Quadrilateral>(Point3d(min.get_x(), max.get_y(), max.get_z()),  dx, -dz, mat)); // top
    sides->add(std::make_shared<Quadrilateral>(Point3d(min.get_x(), min.get_y(), min.get_z()),  dx,  dz, mat)); // bottom

    return sides;
}

class Triangle: public Hittable {
    // TODO
};

#endif //RAY_TRACING_GEOMETRY_H
