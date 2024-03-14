#include <iostream>

#include "vector.h"
#include "color.h"
#include "ray.h"

bool hit_sphere(const Point3d& center, double radius, const Ray& ray) {
    Vector3d oc = ray.origin() - center;
    auto a = dot(ray.direction(), ray.direction());
    auto b = 2.0 * dot(oc, ray.direction());
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    return (discriminant >= 0);
}

Color ray_color_hit_red(const Ray& r) {
    if (hit_sphere(Point3d(0,0,-1), 0.5, r))
        return Color(1, 0, 0);
    Vector3d unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.get_y() + 1.0);
    return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
}

Color ray_color_gradiant(const Ray& r) {
    Vector3d unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.get_y() + 1.0);
    return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
}


int main() {

    // Image

    double aspect_ratio = 16. / 9.;
    int image_width = 256;
    int image_height = static_cast<int>(1/aspect_ratio * image_width);
    image_height = (image_height < 1) ? 1: image_height;

    // Camera

    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);
    Point3d camera_center = Point3d(0., 0., 0.);
    Point3d viewport_u =  Vector3d(viewport_width, 0., 0.);
    Point3d viewport_v =  Vector3d(0., -viewport_height, 0.);
    Point3d pixel_delta_u = viewport_u / image_width;
    Point3d pixel_delta_v = viewport_v / image_height;
    Point3d viewport_upper_left = camera_center - Point3d (0., 0., focal_length) - viewport_u/2 - viewport_v/2;
    Point3d pixel_00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            Point3d pixel_center = pixel_00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            Vector3d ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            Color pixel_color = ray_color_gradiant(r);
            write_color(std::out, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";

}