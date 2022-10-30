#!/bin/bash
# build miniob

rmm -rf build
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=ON .. 
make $@