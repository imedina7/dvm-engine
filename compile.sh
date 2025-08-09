#!/bin/bash
mkdir -p build
pushd build
cmake -S ../ -B .
make && make Shaders && ./DvmEngine
popd