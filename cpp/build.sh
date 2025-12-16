#!/bin/bash

set -ex

DEVICE=$1
BUILD_TYPE=$2

if [ -n "$BUILD_TYPE" ]; then
  BUILD_TYPE=Release
else
  BUILD_TYPE=Debug
fi

rm -rf ./build/* > /dev/null 2>&1

if [ "$DEVICE" == "host" ]; then
  echo "Build for x86_64"
  conan install . --build=missing --output-folder=build -s build_type=$BUILD_TYPE
else
  echo "Build for device"
  conan install . --build=missing --profile=$DEVICE --output-folder=build -s build_type=$BUILD_TYPE
fi

cd build/
source conanbuild.sh
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . -j $(nproc)
cd -
