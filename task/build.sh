#!/bin/sh -x

ROOT_DIR=$(dirname $0)/..

cd ${ROOT_DIR}/

# build examples
cd example
scons
