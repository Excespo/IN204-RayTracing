cmake_minimum_required ( VERSION 3.1.0...3.27.0 )
project(ray_tracing)

# Set to C++11
set ( CMAKE_CXX_STANDARD          11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# In case compilation cannot be done on +WINDOWS -CLION
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Math
    ${CMAKE_SOURCE_DIR}/Geometry
    ${CMAKE_SOURCE_DIR}/Materials
    ${CMAKE_SOURCE_DIR}/Utilities
    ${CMAKE_SOURCE_DIR}/Camera
)

add_executable(ray_tracing main.cpp
        Utilities/utils.h
        Utilities/args.h
        Utilities/clipp.h
        Utilities/color.h
        Utilities/common.h
        Utilities/debug.h
        Utilities/svpng.inc
        Utilities/json.hpp
        Geometry/load_scene.h
        Math/interval.h
        Math/ray.h
        Math/vector.h
        Materials/material.h
        Materials/texture.h
        Camera/camera.h
        Geometry/geometry.h
        Geometry/aabb.h
        Geometry/hittable.h
)