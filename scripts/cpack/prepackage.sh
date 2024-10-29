#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build
DRIVER_SRC_PATH=$(find /usr/src -name "falco-*")

cd $DIR && rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
cp -a $DIR/../../bin $DIR/$BUILD_DIR/bin
mkdir -p $DIR/$BUILD_DIR/etc/falco && touch $DIR/$BUILD_DIR/etc/falco/falco.yaml
mkdir -p $DIR/$BUILD_DIR/lib && cp -a /usr/lib/sysflow $DIR/$BUILD_DIR/lib
mkdir -p $DIR/$BUILD_DIR/driver && cp -a /usr/bin/falcoctl $DIR/$BUILD_DIR/driver/.
mkdir -p $DIR/$BUILD_DIR/src && cp -a $DRIVER_SRC_PATH $DIR/$BUILD_DIR/src/$(basename $DRIVER_SRC_PATH)
mkdir -p $DIR/$BUILD_DIR/modules/src/dkms && cp -a $DIR/../../modules/src/dkms/*  $DIR/$BUILD_DIR/modules/src/dkms
mkdir -p $DIR/$BUILD_DIR/modules/bin && cp -a $(which make)  $DIR/$BUILD_DIR/modules/bin/make
cp -a $DIR/../service/systemd $DIR/$BUILD_DIR/service
cp $DIR/../service/bin/sysflow $DIR/$BUILD_DIR/bin/.
cp -a $DIR/../service/conf $DIR/$BUILD_DIR/conf
cp $DIR/../service/driver/start $DIR/$BUILD_DIR/driver/.
cp $DIR/../service/driver/cleanup $DIR/$BUILD_DIR/driver/.
cp $DIR/../../LICENSE.md $DIR/$BUILD_DIR/.
cp $DIR/../../README.md $DIR/$BUILD_DIR/.

# strip binaries
find "$BUILD_DIR" -type f -name "sysporter" -exec strip -g '{}' \;

