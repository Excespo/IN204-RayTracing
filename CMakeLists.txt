cmake_minimum_required(VERSION 3.27)
project(ray_tracing)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(ray_tracing main.cpp
        utils.h
        vector.h
        debug.h
        ray.h
        color.h
        interval.h
        camera.h
        hittable.h
        material.h
        geometry.h
        common.h
)

add_executable(test_vector
        vector.h
        test/test_vector.cpp
)