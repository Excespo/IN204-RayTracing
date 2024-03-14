//
// Created by LUO Yijie on 2024/3/14.
//

#ifndef RAY_TRACING_COLOR_H
#define RAY_TRACING_COLOR_H

#include "vector.h"
#include <iostream>

using Color = Vector3d;

void write_color (std::ostream os, Color pixel_color) {
    os << static_cast<int>(255.999 * pixel_color.get_r()) << " "
       << static_cast<int>(255.999 * pixel_color.get_g()) << " "
       << static_cast<int>(255.999 * pixel_color.get_b()) << "\n"
}

#endif //RAY_TRACING_COLOR_H
