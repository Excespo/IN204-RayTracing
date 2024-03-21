cmake_minimum_required ( VERSION 3.1.0...3.27.0 )
project(ray_tracing)

# Set to C++11
set ( CMAKE_CXX_STANDARD          11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# In case compilation cannot be done on +WINDOWS -CLION
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
        clipp.h
        args.h
        aabb.h
        texture.h
)