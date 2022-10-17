#!/bin/bash
# git clone --recurse-submodules https://github.com/szu-dseg/miniob.git

PROj_HOME=`pwd`

# build libevent
cd $PROj_HOME/deps/libevent
mkdir build && cd build
cmake .. -DEVENT__DISABLE_OPENSSL=ON
make
sudo make install

# build google test
cd $PROj_HOME/deps/googletest
mkdir build && cd build
cmake ..
make
sudo make install

# build jsoncpp
cd $PROj_HOME/deps/jsoncpp
mkdir build && cd build
cmake -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF ..
make
sudo make install