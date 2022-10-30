#!/bin/bash
# 以release模式编译

rm -rf build
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=ON .. 
make $@