#include <iostream>
#include <fstream>

#include "common.h"
#include "camera.h"
#include "material.h"
#include "geometry.h"

// Only function to be modified by users
HittableList construct() {
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Color(0.2, 0.2, 0.2));
    world.add(std::make_shared<Sphere>(Point3d(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Point3d center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - Point3d(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3d(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3d(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3d(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, char** argv) {

    // Read and write ray-tracing arguments
    Args args;
    auto cli = (
            value("output_file", args.output_file).doc("output image to OUTPUT_FILE"),
            option("-m", "-message").doc("MESSAGE written to result/log.txt")
                & value("MESSAGE", args.message),
            option("-p", "--parallel").set(args.parallel).doc("run ray-tracing program in parallelization"),
            option("-a", "--antialias").set(args.anti_alias).doc("enable anti-aliasing with trivial sampling"),
            option("-r", "-aspect_ratio").doc("aspect ratio of image")
                & value("ASPECT_RATIO", args.aspect_ratio),
            option("-s", "-samples_per_pixel").doc("number of samples per pixel")
                & value("N_SAMPLES", args.samples_per_pixel),
            option("-w", "-width").doc("width of image")
                & value("IMAGE_WIDTH", args.image_width),
            option("-d", "-depth").doc("maxi depth of recursion of rays")
                & value("MAX_DEPTH", args.max_depth),
            option("-n", "num_threads").doc("number of threads to activate")
                & value("NUM_THREADS", args.num_threads)
            );
    if(!parse(argc, argv, cli)) std::clog << make_man_page(cli, argv[0]);
    args.print(std::clog);

    std::ofstream file(args.message_to_file, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << args.message_to_file << std::endl;
        return 1;
    }
    file << "\n\n" << args.output_file << ":\n\t" << args.message << std::endl;
    args.print(file);
    file.close();

    // Construct all world
    HittableList world = construct();

    // Initialize camera
    Camera cam;
    cam.aspect_ratio      = args.aspect_ratio;
    cam.image_width       = args.image_width;
    cam.samples_per_pixel = args.samples_per_pixel;
    cam.max_depth         = args.max_depth;
    cam.vertical_fov      = 20;
    cam.look_from         = Point3d(13,2,3);
    cam.look_at           = Point3d(0,0,0);
    cam.vec_up            = Vector3d(0,1,0);
    cam.defocus_angle     = 0.6;
    cam.focus_dist        = 10.0;

    cam.rp.use_anti_alias = args.anti_alias;
    cam.rp.use_parallel   = args.parallel;
    cam.rp.output         = args.output_file;
    cam.rp.num_threads    = args.num_threads;
    cam.rp.log            = args.message_to_file;

    // Trace!
    cam.render(world);
}
