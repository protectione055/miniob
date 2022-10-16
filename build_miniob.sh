#!/bin/bash

# build miniob
cd $PROj_HOME
mkdir build && cd build
cmake .. -DDEBUG=ON
make