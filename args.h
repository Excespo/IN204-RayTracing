//
// Created by LUO Yijie on 2024/3/20.
//

#ifndef RAY_TRACING_ARGS_H
#define RAY_TRACING_ARGS_H

#include <cstring>
#include <iostream>
#include "clipp.h"
#include "vector.h"
using std::string;
using namespace clipp;

class Args {
public:
    string output_file = "image.png";
    bool parallel = true;
    int num_threads = 8;
    bool anti_alias = true;
    string message;
    string message_to_file = "result/log.txt"; // with script
//    string message_to_file = "../result/log.txt"; // with CLion

    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 10;
    int max_depth = 10;
    double vertical_fov = 20;
    Point3d look_from = Point3d(0,0,0);
    Point3d look_at   = Point3d(0,0,-1);
    Point3d vec_up      = Vector3d(0,1,0);
    double defocus_angle = 0;
    double focus_dist    = 10;

    void print(std::ostream& os) const {
        os << "************ ARGS OF RAY-TRACER ************\n";
        os << "RESOLUTION : (" << image_width << " x " << int(image_width/aspect_ratio) << "), "
                  << "SAMPLES_PER_PIXEL :"  << samples_per_pixel << ", "
                  << "MAX_DEPTH : " << max_depth << "\n";
        os << "OUTPUT TO : " << output_file << ", "
                  << num_threads << " threads-PARALLEL : " << (parallel ? "ON" : "OFF") << ", "
                  << "ANTI-ALIAS : " << (anti_alias ? "ON" : "OFF") << ", ";
        os << "\n********************************************\n";
    }
};




#endif //RAY_TRACING_ARGS_H
