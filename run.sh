#!/bin/bash

cmake -B build
cmake --build build

echo -e "\nPlaying ray tracing ..."
build/ray_tracing > image.ppm