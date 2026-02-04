@echo off

del build
mkdir build

cmake -S . -B build
cmake --build build
