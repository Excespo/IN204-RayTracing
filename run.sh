#!/bin/bash

cmake -B build
cmake --build build

echo "\nPlaying ray tracing ..."
build/ray_tracing > image.ppm