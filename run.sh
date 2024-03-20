#!/bin/bash

cmake -B build
cmake --build build

echo -e "\nPlaying ray tracing ..."
mkdir -p result
MESSAGE_TO="result/log.txt"
touch ${MESSAGE_TO}

output_file="result/image.ppm"
MESSAGE="30 samples p+a"
N_SAMPLES=30
MAX_DEPTH=10

build/ray_tracing ${output_file} -p -a -m ${MESSAGE} -s ${N_SAMPLES} -d ${MAX_DEPTH}
