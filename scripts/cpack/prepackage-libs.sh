#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build-libs

cd $DIR && rm -rf build && mkdir -p build
# copy from libs image
mkdir -p $DIR/build/lib/falcosecurity && cp -a $DIR/../../modules/bin/falco-driver-loader $DIR/build/driver/.
# copy from libs image
mkdir -p $DIR/build/include/falcosecurity && cp -a $DRIVER_SRC_PATH $DIR/build/driver/src/$(basename $DRIVER_SRC_PATH)
# copy from sf-collector libs phase
mkdir -p $DIR/build/lib/sysflow && cp -a $DIR/../../modules/bin/falco-driver-loader $DIR/build/driver/.
# copy from sf-collector libs phase
mkdir -p $DIR/build/lib/sysflow && cp -a $DIR/../../modules/bin/falco-driver-loader $DIR/build/driver/.
cp $DIR/../../LICENSE.md $DIR/build/.
cp $DIR/../../README.md $DIR/build/.
cd $DIR

