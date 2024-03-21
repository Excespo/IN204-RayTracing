//
// Created by LUO Yijie on 2024/2/18.
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
    Interval(const Interval& in1, const Interval& in2)
        : min(fmin(in1.get_min(), in2.get_min())), max(fmax(in1.get_max(), in2.get_max())) {}

    [[nodiscard]] double get_min() const { return min; }
    [[nodiscard]] double get_max() const { return max; }
    void set_min(double m) { min = m; }
    void set_max(double m) { max = m; }

    [[nodiscard]] double size() const {
        return max - min;
    }

    Interval expand(double delta) const {
        double side = delta / 2;
        return Interval(min - side, max + side);
    }

    [[nodiscard]] bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
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

Interval operator+(const Interval& in, double displacement) {
    return Interval(in.get_min() + displacement, in.get_max() + displacement);
}

Interval operator+(double displacement, const Interval& in) {
    return in + displacement;
}

#endif //RAY_TRACING_INTERVAL_H
