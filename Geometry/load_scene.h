//
// Created by JI Zihe on 2024/3/20
//

#pragma once
#include "json.hpp"

#include "common.h"
#include "camera.h"
#include "material.h"
#include "geometry.h"
#include "texture.h"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::shared_ptr<Texture> parse_texture(const json& tex_json) {
    if (tex_json["type"] == "Checker") {
        double scale = tex_json["scale"];
        Color even(tex_json["even"][0], tex_json["even"][1], tex_json["even"][2]);
        Color odd(tex_json["odd"][0], tex_json["odd"][1], tex_json["odd"][2]);
        return std::make_shared<CheckerTexture>(scale, even, odd);
    }
    throw std::runtime_error("Unknown texture type");
}

std::shared_ptr<Material> parse_material(const json& mat_json) {
    if (mat_json["type"] == "Lambertian") {
        // Check if the key "color" exists in the json object
        if (!mat_json.contains("color")) {
            std::cout << "Texture detected" << std::endl;
            auto texture = parse_texture(mat_json["texture"]);
            return std::make_shared<Lambertian>(texture);
        } else {
            Color color(mat_json["color"][0], mat_json["color"][1], mat_json["color"][2]);
            return std::make_shared<Lambertian>(color);
        }
    } else if (mat_json["type"] == "Metal") {
        double fuzz = mat_json.value("fuzz", 0.0); // Provide default value if not specifie
            Color color(mat_json["color"][0], mat_json["color"][1], mat_json["color"][2]);
            return std::make_shared<Metal>(color, fuzz);
    } else if (mat_json["type"] == "Dielectric") {
        double ref_idx = mat_json["ref_idx"];
        return std::make_shared<Dielectric>(ref_idx);
    }
    throw std::runtime_error("Unknown material type");
}

HittableList load_scene(const std::string& filename) {
    std::ifstream file(filename);
    json scene;
    file >> scene;

    HittableList world;

    for (const auto& obj : scene["Objects"]) {
        if (obj["type"] == "Sphere") {
            Point3d center(obj["center"][0], obj["center"][1], obj["center"][2]);
            double radius = obj["radius"];
            auto material = parse_material(obj["material"]);
            world.add(std::make_shared<Sphere>(center, radius, material));
        } else if (obj["type"] == "Quadrilateral") {
            Point3d vertex(obj["vertex"][0], obj["vertex"][1], obj["vertex"][2]);
            Vector3d edge1(obj["edge1"][0], obj["edge1"][1], obj["edge1"][2]);
            Vector3d edge2(obj["edge2"][0], obj["edge2"][1], obj["edge2"][2]);
            auto material = parse_material(obj["material"]);
            world.add(std::make_shared<Quadrilateral>(vertex, edge1, edge2, material));
        } else if (obj["type"] == "Triangle") {
            Point3d vertex1(obj["v1"][0], obj["v1"][1], obj["v1"][2]);
            Point3d vertex2(obj["v2"][0], obj["v2"][1], obj["v2"][2]);
            Point3d vertex3(obj["v3"][0], obj["v3"][1], obj["v3"][2]);
            auto material = parse_material(obj["material"]);
            world.add(std::make_shared<Triangle>(vertex1, vertex2, vertex3, material));
        }
    }
    return world;
}