#!/bin/sh -x

ROOT_DIR=$(dirname $0)/..

cd ${ROOT_DIR}/

# clean up examples
cd example
scons --clean
