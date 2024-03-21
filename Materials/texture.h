//
// Created by LUO Yijie on 2024/3/21.
//

#ifndef RAY_TRACING_TEXTURE_H
#define RAY_TRACING_TEXTURE_H

#include <utility>

#include "vector.h"
#include "color.h"

class Texture {
public:
    virtual ~Texture() = default;
    virtual Color value(double u, double v, const Point3d& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor(const Color& albedo) : albedo(albedo) {}
    SolidColor(double red, double green, double blue)
        : SolidColor(Color(red, green, blue)) {}
    Color value(double u, double v, const Point3d& p) const override {
        return albedo;
    }
private:
    Color albedo;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : inv_scale(scale), even(std::move(even)), odd(std::move(odd)) {}
    CheckerTexture(double scale, const Color& c1, const Color& c2)
        : inv_scale(scale), even(make_shared<SolidColor>(c1)), odd(make_shared<SolidColor>(c2)) {}
    Color value(double u, double v, const Point3d& p) const override {
        auto xInt = int(std::floor(inv_scale * p.get_x()));
        auto yInt = int(std::floor(inv_scale * p.get_y()));
        auto zInt = int(std::floor(inv_scale * p.get_z()));
        bool isEven = (xInt + yInt + zInt) % 2 == 0;
        return isEven ? even->value(u,v,p) : odd->value(u,v,p);
    }
private:
    double inv_scale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

#endif //RAY_TRACING_TEXTURE_H
