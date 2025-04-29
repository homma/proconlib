#!/bin/sh -x

ROOT_DIR=$(dirname $0)/..

cd ${ROOT_DIR}/

clang-format -i *.hpp

# format examples
cd example
clang-format -i *.cpp
