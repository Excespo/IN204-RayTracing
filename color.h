//
// Created by LUO Yijie on 2024/3/14.
//

#ifndef RAY_TRACING_COLOR_H
#define RAY_TRACING_COLOR_H

#include <iostream>
#include "utils.h"

using Color = Vector3d;

double linear_to_gamma(double linear) {
    if (linear > 0)
        return std::sqrt(linear);
    return 0;
}

void write_color(std::ostream& os, const Color& pixel_color) {
    double r = linear_to_gamma(pixel_color.get_x());
    double g = linear_to_gamma(pixel_color.get_y());
    double b = linear_to_gamma(pixel_color.get_z());
    static const Interval intensity_bound(0.000, 0.999);
    r = int(256 * intensity_bound.clamp(r));
    g = int(256 * intensity_bound.clamp(g));
    b = int(256 * intensity_bound.clamp(b));

    os << r << " " << g << " " << b << "\n";
}

#endif //RAY_TRACING_COLOR_H
