#!/bin/bash

cmake -B build
cmake --build build

echo -e "\nPlaying ray tracing ..."
mkdir -p result
MESSAGE_TO="result/log.txt"
touch ${MESSAGE_TO}

n_samples=10
max_depth=10
image_width=1280
n_threads=8

output_name="result/image_${image_width}_${n_samples}"
extension="png"
output_file="${output_name}.${extension}"
#output_file="cout"

MESSAGE="10samples on 4 threads, 1280, with quad on spheres"

build/ray_tracing ${output_file} -m "${MESSAGE}" \
                  -s ${n_samples} -d ${max_depth} -w ${image_width} -a \
                  -p -n ${n_threads}
