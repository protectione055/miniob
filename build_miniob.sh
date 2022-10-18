#!/bin/bash
# build miniob

cd build
cmake .. -DDEBUG=ON
make -j && make install -j
cd ..
