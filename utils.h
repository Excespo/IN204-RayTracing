//
// Created by 18490 on 2024/3/13.
//

#ifndef RAY_TRACING_UTILS_H
#define RAY_TRACING_UTILS_H

#include <cmath>
#include <memory>
#include <limits>
#include <random>

using std::fmin;
using std::fmax;

const double inf = std::numeric_limits<double>::infinity();
const double pi = 3.1415936535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180;
}

inline double random_double() {
    // generate a random real in [0, 1)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}


#endif //RAY_TRACING_UTILS_H
