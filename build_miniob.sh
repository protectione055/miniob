#!/bin/bash
# build miniob

PROj_HOME=`pwd`

cd $PROj_HOME
mkdir -p build && cd build
cmake .. -DDEBUG=ON
make $@
