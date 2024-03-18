//
// Created by LUO Yijie on 2024/2/18.
//

#ifndef RAY_TRACING_CAMERA_H
#define RAY_TRACING_CAMERA_H

#include <iostream>
#include <iomanip>

#include "common.h"
#include "hittable.h"
#include "material.h"

class Camera {
    //TODO MAKE ALL PARAMS CONFIGURABLE, INCLUDING IS_ANTIALIASING, IS_DEFOCUS_BLUR
public:
    double aspect_ratio      = 1.0;  // Ratio of image width over height
    int    image_width       = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth         = 10;   // Maximum number of ray bounces into scene

    double vertical_fov     = 90;              // Vertical view angle (field of view)
    Point3d look_from = Point3d(0,0,0);  // Point camera is looking from
    Point3d look_at   = Point3d(0,0,-1);   // Point camera is looking at
    Vector3d vec_up      = Vector3d(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera look_from point to plane of perfect focus

    void print_config() const {
        std::clog << "********** CAMERA WITH CONFIGS **********\n";
        std::clog << "Resolution: (" << image_width << "x" << int(image_width / aspect_ratio) << "), "
                  << "Vertical Field of View: " << vertical_fov << "\n"
                  << "Looking from " << look_from << ", " // Little reminder here that look_from is a const ref, so former override of << didnt work
                  << "Looking at " << look_at << ", "
                  << "Up vector " << vec_up << "\n";
    }

    void render(const Hittable& object) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        const int progressBarWidth = 50;
        for (int j = 0; j < image_height; ++j) {
            // Progress bar
            double progress = double(j) / image_height;
            int pos = int(progressBarWidth * progress);
            std::clog << "\r[";
            for (int k = 0; k < progressBarWidth; ++k) {
                if (k < pos) std::clog << "=";
                else if (k == pos) std::clog << ">";
                else std::clog << " ";
            }
            std::clog << "] " << int(progress * 100.0) << "% " << "Lines Remaining: " << (image_height - j) << " ";
            std::clog << std::flush; // 确保立即输出

            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray ray = get_ray(i, j);
                    pixel_color += ray_color(ray, max_depth, object);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << std::endl << "Done.\n";
    }

private:
    int    image_height;    // Rendered image height
    double pixel_samples_scale;
    Point3d center;          // Camera center
    Point3d pixel_00_loc;     // Location of pixel 0, 0
    Vector3d   pixel_delta_u;   // Offset to pixel to the right
    Vector3d   pixel_delta_v;   // Offset to pixel below
    Vector3d   u, v, w;         // Camera frame basis vectors
    Vector3d   defocus_disk_u;  // Defocus disk horizontal radius
    Vector3d   defocus_disk_v;  // Defocus disk vertical radius

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1. / samples_per_pixel;
        center = look_from;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vertical_fov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        // w - backward of camera, u - right hand side of camera, v - upside of camera
        // With convention in online course https://www.bilibili.com/video/BV1X7411F744/?spm_id_from=333.337.search-card.all.click
        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vec_up, w));
        v = cross(w, u);
        std::clog << "Coordinates of camera are:\n";
        std::clog << "w: " << w << ",u: " << u << ",v: " << v << "\n";

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vector3d viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        Vector3d viewport_v = viewport_height * (-v);  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors to the next pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel_00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j
        // TODO HERE ADD CONTROL ON RANDOM ANTIALIASING AND BLURING
        // TODO MORE REAL SIMULATION:
        //  - SHOULD CONTAIN MORE RAYS FROM NOT ONLY THE EXACT CENTER OF CAMERA
        //  - SHAPE OF CAMERA
        //  - SELF-ADAPTED SAMPLING?
        //  - MORE PARAMS TO CONTROL THE BLUR?
        auto offset = pixel_sample_square();
        auto pixel_sample = pixel_00_loc
                            + ((i + offset.get_x()) * pixel_delta_u)
                            + ((j + offset.get_y()) * pixel_delta_v);
        auto ray_origin = (defocus_angle <= 0) ? center: defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vector3d pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        return Vector3d(random_double() - 0.5, random_double() - 0.5, 0);
    }

    Point3d defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    Color ray_color(const Ray& ray, int depth, const Hittable& hittable_obj) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Color(0,0,0);

        HitStatus stat;

        if (hittable_obj.hit(ray, Interval(0.001, inf), stat)) {
            Ray scattered;
            Color attenuation;
            if (stat.material->scatter(ray, stat, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, hittable_obj);
            return Color(0,0,0);
        }

        // TODO HERE WHEN MISS THE HIT WE MODEL THE COLOR TO BACKGROUND COLOR, WHICH IS FIXED, BUT SHOULD BE MORE FLEXIBLE
        Vector3d unit_direction = unit_vector(ray.direction());
        auto a = 0.5*(unit_direction.get_y() + 1.0);
        return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
    }
};

#endif //RAY_TRACING_CAMERA_H
