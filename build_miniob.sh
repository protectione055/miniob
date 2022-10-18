#!/bin/bash
# build miniob

cd build
cmake .. -DDEBUG=ON
make clean && make -j && make install -j
cd ..
