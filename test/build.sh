#!/bin/bash

set -eu

GOOGLETEST_VERSION="v1.14.0"
GOOGLEDIR="googletest-1.14.0"

mkdir -p third_party/
pushd third_party/
mkdir gtest
wget --quiet https://github.com/google/googletest/archive/refs/tags/${GOOGLETEST_VERSION}.tar.gz
tar -xf ${GOOGLETEST_VERSION}.tar.gz
rm ${GOOGLETEST_VERSION}.tar.gz
cd ${GOOGLEDIR}
mkdir build/
cd build/
GPATH=$(pwd)
cmake .. -DCMAKE_INSTALL_PREFIX=$GPATH/../../gtest
make
make install

# rm -rf build/
# mkdir build/
# cd build/
# cmake ..
# make
