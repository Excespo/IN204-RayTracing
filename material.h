//
// Created by 18490 on 2024/3/18.
//

#ifndef RAY_TRACING_MATERIAL_H
#define RAY_TRACING_MATERIAL_H

#include "common.h"
#include "hittable.h"

class Material {
public:
    virtual ~Material() = default;
    virtual bool scatter(const Ray& ray_in, const HitStatus& stat, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(a) {}

    bool scatter(const Ray& r_in, const HitStatus& stat, Color& attenuation, Ray& scattered)
    const override {
        auto scatter_direction = stat.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = stat.normal;

        scattered = Ray(stat.hit_point, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    Color albedo;
};


class Metal : public Material {
public:
    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray& r_in, const HitStatus& stat, Color& attenuation, Ray& scattered)
    const override {
        Vector3d reflected = reflect(unit_vector(r_in.direction()), stat.normal);
        scattered = Ray(stat.hit_point, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), stat.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};


class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& ray_in, const HitStatus& stat, Color& attenuation, Ray& scattered)
    const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = stat.front_face ? (1.0/ir) : ir;

        Vector3d unit_direction = unit_vector(ray_in.direction());
        double cos_theta = fmin(dot(-unit_direction, stat.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vector3d direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, stat.normal);
        else
            direction = refract(unit_direction, stat.normal, refraction_ratio);

        scattered = Ray(stat.hit_point, direction);
        return true;
    }

private:
    double ir; // Index of Refraction

    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

#endif //RAY_TRACING_MATERIAL_H
