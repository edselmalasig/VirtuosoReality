#!/bin/bash
cd glew-2.0.0
make
cd ..

cd glfw-3.2.1
mkdir build
cd build
cmake ..
make

cd ..
cd ..

cd portaudio
mkdir build
cd build
cmake ..
make

cd ..
cd ..

brew install scons
cd libaudiodecoder
scons