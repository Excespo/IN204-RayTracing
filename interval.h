//
// Created by 18490 on 2024/3/18.
//

#ifndef RAY_TRACING_INTERVAL_H
#define RAY_TRACING_INTERVAL_H

#include "utils.h"

class Interval {
private:
    double min, max;

public:
    Interval() : min(+inf), max(-inf) {}
    Interval(double min, double max) : min(min), max(max) {}

    [[nodiscard]] double get_min() const { return min; }
    [[nodiscard]] double get_max() const { return max; }

    [[nodiscard]] double size() const {
        return max - min;
    }

    [[nodiscard]] bool contains(double x) const {
        return min <= x && x <= max;
    }

    [[nodiscard]] double clamp(double x) const  {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    static const Interval empty, universe;

};

const Interval Interval::empty = Interval(inf, -inf);
const Interval Interval::universe = Interval(-inf, inf);

#endif //RAY_TRACING_INTERVAL_H
