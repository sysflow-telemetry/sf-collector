#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build-driver
DRIVER_SRC_PATH=$(find /usr/src -name "falco-*")

cd $DIR && rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
# change this to copy from runtime image
mkdir -p $DIR/$BUILD_DIR/driver && cp -a $DIR/../../modules/bin/falco-driver-loader $DIR/$BUILD_DIR/driver/.
mkdir -p $DIR/$BUILD_DIR/driver/src && cp -a $DRIVER_SRC_PATH $DIR/$BUILD_DIR/driver/src/$(basename $DRIVER_SRC_PATH)
# change this to copy from runtime image
mkdir -p $DIR/$BUILD_DIR/modules/src && cp -a $DIR/../../modules/src/dkms  $DIR/$BUILD_DIR/modules/src/.
cd $DIR

