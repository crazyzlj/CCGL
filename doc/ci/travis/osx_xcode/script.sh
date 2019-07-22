#!/bin/sh

set -e
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DUNITTEST=1 -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ ..
make -j4
make install
cd ../bin
./UnitTests_CCGL
cd ..
ls

