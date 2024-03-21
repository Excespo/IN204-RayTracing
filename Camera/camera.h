//
// Created by LUO Yijie on 2024/2/18.
//

#ifndef RAY_TRACING_CAMERA_H
#define RAY_TRACING_CAMERA_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>

#include "common.h"
#include "hittable.h"
#include "material.h"
#include "svpng.inc"

class RenderParams {
public:
    bool use_anti_alias;
    bool use_parallel;
    int num_threads;
    string output;
    string log;
    
    RenderParams()
            : use_anti_alias(true), use_parallel(true), num_threads(4), output("cout") {}
    RenderParams(bool uaa, bool up, int n_t, const string& o)
        : use_anti_alias(uaa), use_parallel(up), num_threads(n_t), output(o) {}
};

class Camera {
    //TODO MAKE ALL PARAMS CONFIGURABLE, INCLUDING IS_ANTIALIASING, IS_DEFOCUS_BLUR
public:
//    friend class RenderStrategy;
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int image_width = 100;  // Rendered image width in pixel count
    int samples_per_pixel = 10;   // Count of random samples for each pixel
    int max_depth = 10;   // Maximum number of ray bounces into scene

    double vertical_fov = 90;              // Vertical view angle (field of view)
    Point3d look_from = Point3d(0,0,0);  // Point camera is looking from
    Point3d look_at = Point3d(0,0,-1);   // Point camera is looking at
    Vector3d vec_up = Vector3d(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera look_from point to plane of perfect focus
    
    RenderParams rp;

    void render(const Hittable& world) {
        if (rp.output == "cout") {
            renderToCOUT(world);
        } else {
            string extension = rp.output.substr(rp.output.size() - 4);
            if (extension == ".ppm") {
                if (rp.use_parallel) {
                    renderToPPM_parallel(world, rp.output, rp.num_threads);
                } else {
                    renderToPPM(world, rp.output);
                }
            } else if (extension == ".png") {
                if (rp.use_parallel) {
                    renderToPNG_parallel(world, rp.output, rp.num_threads);
                } else {
                    renderToPNG(world, rp.output);
                }
            } else {
                std::cerr << "Unsupported file format. Please use .ppm or .png" << std::endl;
            }
        }
    }

private:
    int image_height;    // Rendered image height
    double pixel_samples_scale;
    Point3d center;          // Camera center
    Point3d pixel_00_loc;     // Location of pixel 0, 0
    Vector3d pixel_delta_u;   // Offset to pixel to the right
    Vector3d pixel_delta_v;   // Offset to pixel below
    Vector3d u, v, w;         // Camera frame basis vectors
    Vector3d defocus_disk_u;  // Defocus disk horizontal radius
    Vector3d defocus_disk_v;  // Defocus disk vertical radius;

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
        std::clog << "w: " << w << "\nu: " << u << "\nv: " << v << "\n";

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

    Ray get_ray(int i, int j, bool use_anti_alias) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j
        // TODO HERE ADD CONTROL ON RANDOM ANTIALIASING AND BLURING
        // TODO MORE REAL SIMULATION:
        //  - SHOULD CONTAIN MORE RAYS FROM NOT ONLY THE EXACT CENTER OF CAMERA
        //  - SHAPE OF CAMERA
        //  - SELF-ADAPTED SAMPLING?
        //  - MORE PARAMS TO CONTROL THE BLUR?
        auto offset = (use_anti_alias) ? pixel_sample_square() : Vector3d(0, 0, 0);
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

    Color ray_color(const Ray& ray, int depth, const Hittable& obj) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Color(0,0,0);

        HitStatus stat;

        if (obj.hit(ray, Interval(0.001, inf), stat)) {
            Ray scattered;
            Color attenuation;
            if (stat.material->scatter(ray, stat, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, obj);
            return Color(0,0,0);
        }

        // TODO HERE WHEN MISS THE HIT WE MODEL THE COLOR TO BACKGROUND COLOR, WHICH IS FIXED, BUT SHOULD BE MORE FLEXIBLE
        Vector3d unit_direction = unit_vector(ray.direction());
        auto a = 0.5*(unit_direction.get_y() + 1.0);
        return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
    }

    // 将渲染单个区域（一组行）的任务分配给线程
    void renderSection(const Hittable& world, int startY, int endY, std::vector<std::vector<Color>>& linesBuffer, int& completedLines, std::mutex& progressMutex) {
        int progressBarWidth = 50;
        for (int y = startY; y < endY; ++y) {
            for (int x = 0; x < image_width; ++x) {
                Color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray ray = get_ray(x, y, rp.use_anti_alias);
                    pixel_color += ray_color(ray, max_depth, world);
                }
                linesBuffer[y][x] = pixel_samples_scale * pixel_color;
            }
            // Progress update
            {
                std::lock_guard<std::mutex> lock(progressMutex);
                completedLines++;
                // Optional: Progress output can be handled here or elsewhere
                double progress = double(completedLines) / image_height;
                int pos = int(progressBarWidth * progress);
                std::clog << "\r[";
                for (int k = 0; k < progressBarWidth; ++k) {
                    if (k < pos) std::clog << "=";
                    else if (k == pos) std::clog << ">";
                    else std::clog << " ";
                }
                std::clog << "] " << int(progress * 100.0) << "% " << "Lines Remaining: " << (image_height - completedLines) << " ";
                std::clog << std::flush; // Ensure immediate output
            }
        }
    }


    void renderToCOUT(const Hittable& world) {
        initialize();
        ProgressBar pb(image_height);

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            pb.update(j);

            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray ray = get_ray(i, j, rp.use_anti_alias);
                    pixel_color += ray_color(ray, max_depth, world);
                }
                write_color_PPM(std::cout, pixel_samples_scale * pixel_color); // take average of multi-sampling
            }
        }
        pb.done(rp.log);
    }

    void renderToPPM(const Hittable& world, const std::string& filePath) {
        std::ofstream file;
        file.open(filePath);

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return;
        }

        initialize();
        ProgressBar pb(image_height);

        file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            pb.update(j);

            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray ray = get_ray(i, j, rp.use_anti_alias);
                    pixel_color += ray_color(ray, max_depth, world);
                }
                write_color_PPM(file, pixel_samples_scale * pixel_color); // Now writes to file
            }
        }
        file.close();
        pb.done(rp.log);
    }

    void renderToPNG(const Hittable& world, const std::string& filePath) {
        initialize();
        ProgressBar pb(image_height);
        std::vector<unsigned char> pixels(image_width * image_height * 3);

        for (int j = 0; j < image_height; ++j) {
            pb.update(j);

            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray ray = get_ray(i, j, rp.use_anti_alias);
                    pixel_color += ray_color(ray, max_depth, world);
                }
                write_color_PNG(pixels, pixel_samples_scale * pixel_color, (j * image_width + i) * 3);
            }
        }

        FILE* fp = fopen(filePath.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open file " << filePath << " for writing." << std::endl;
            return;
        }
        svpng(fp, image_width, image_height, pixels.data(), 0);
        fclose(fp);
        pb.done(rp.log);
    }

    void renderToPPM_parallel(const Hittable& world, const std::string& filePath, int numThreads) {
    std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return;
        }

        initialize();
        std::clog << "Starting rendering...\n";

        file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        std::vector<std::thread> threads;
        std::vector<std::vector<Color>> linesBuffer(image_height, std::vector<Color>(image_width));

        std::mutex progressMutex;
        int completedLines = 0;

        int linesPerThread = image_height / numThreads;
        for (int i = 0; i < numThreads; ++i) {
            int startLine = i * linesPerThread;
            int endLine = (i + 1) * linesPerThread;
            if (i == numThreads - 1) {
                endLine = image_height; // Ensure the last thread handles all remaining lines
            }
            threads.push_back(std::thread([&, startLine, endLine]() {
                renderSection(world, startLine, endLine, std::ref(linesBuffer), std::ref(completedLines), std::ref(progressMutex));
            }));
        }

        for (auto& t : threads) {
            t.join(); // Wait for all threads to finish
        }

        // Write the lines to the file
        for (const auto& line : linesBuffer) {
            for (const auto& color : line) {
                write_color_PPM(file, color);
            }
        }

        file.close();
        std::clog << "\nRendering complete.\n";
    }


    void renderToPNG_parallel(const Hittable& world, const std::string& filePath, int numThreads) {
        initialize();
        std::vector<std::vector<Color>> linesBuffer(image_height, std::vector<Color>(image_width));
        std::vector<std::thread> threads;
        std::mutex progressMutex;
        int completedLines = 0;

        auto start = std::chrono::high_resolution_clock::now();
        int linesPerThread = image_height / numThreads;
        for (int i = 0; i < numThreads; ++i) {
            int startLine = i * linesPerThread;
            int endLine = (i + 1) * linesPerThread;
            if (i == numThreads - 1) {
                endLine = image_height; // Ensure the last thread handles all remaining lines
            }
            // Utilizing the universal renderSection function
            // threads.emplace_back([&world, startLine, endLine, &linesBuffer, &completedLines, &progressMutex](){
            //     renderSection(world, startLine, endLine, linesBuffer, completedLines, progressMutex);
            // });
            threads.push_back(std::thread([&, startLine, endLine]() {
                renderSection(world, startLine, endLine, std::ref(linesBuffer), std::ref(completedLines), std::ref(progressMutex));
            }));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // After rendering, convert the Color data in linesBuffer to pixel data for the PNG
        std::vector<unsigned char> pixels(image_width * image_height * 3);
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                int index = (j * image_width + i) * 3;
                auto& color = linesBuffer[j][i];
                write_color_PNG(pixels, color, index); // Assuming write_color_PNG can directly accept a Color object
            }
        }

        // Write the pixel data to a PNG file
        FILE* fp = fopen(filePath.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open file " << filePath << " for writing." << std::endl;
            return;
        }
        svpng(fp, image_width, image_height, pixels.data(), 0);
        fclose(fp);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::clog << std::endl << "Done.\n";
        std::clog << "Time elapsed: " << elapsed.count() << "s" << std::endl;
    }

};

#endif //RAY_TRACING_CAMERA_H
