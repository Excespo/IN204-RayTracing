//
// Created by LUO Yijie on 2024/3/13.
//

#ifndef RAY_TRACING_UTILS_H
#define RAY_TRACING_UTILS_H

#include <cmath>
#include <memory>
#include <limits>
#include <random>
#include <iostream>
#include <chrono>

using std::fmin;
using std::fmax;
using std::fabs;
using std::sqrt;
using std::shared_ptr;
using std::make_shared;

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

inline int random_int(int min, int max) {
    return int(random_double(min, max+1));
}

class ProgressBar {
public:
    explicit ProgressBar(int total_steps) : total_steps(total_steps), start_time(std::chrono::high_resolution_clock::now()) {}

    void update(int current_step) {
        double progress = double(current_step) / total_steps;
        int pos = int(progressBarWidth * progress);

        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        double total_time_est = elapsed_seconds / progress;
        int remaining_time = int((1.0 - progress) * total_time_est);

        std::clog << "\r[";
        for (int k = 0; k < progressBarWidth; ++k) {
            if (k < pos) std::clog << "=";
            else if (k == pos) std::clog << ">";
            else std::clog << " ";
        }
        std::clog << "] " << int(progress * 100.0) << "% ";
        std::clog << "Time Remaining: " << remaining_time / 60 << "m " << remaining_time % 60 << "s | ";
        std::clog << "Iter Remaining: " << (total_steps - current_step) << " ";
        std::clog << std::flush;
    }

    void done(const std::string& log) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time- start_time).count();

        std::clog << "touching " << log << "\n";
        std::ofstream file(log, std::ios::out | std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << log << std::endl;
            exit(0);
        }
        file << "Elapsed time: " << duration << " " << std::endl;
        file.close();

        std::clog << std::endl << "Done. " << "Elapsed time: " << duration << "s " << std::endl;
    }

private:
    int total_steps;
    const int progressBarWidth = 40;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif //RAY_TRACING_UTILS_H
