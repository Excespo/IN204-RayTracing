cmake_minimum_required(VERSION 3.27)
project(ray_tracing)

set(CMAKE_CXX_STANDARD 17)

add_executable(ray_tracing main.cpp
        constants.h
        vector.h
        debug.h
        ray.h)
