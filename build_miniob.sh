#!/bin/bash
# build miniob

PROj_HOME=`pwd`

cd $PROj_HOME
mkdir build && cd build
cmake .. -DDEBUG=ON
make