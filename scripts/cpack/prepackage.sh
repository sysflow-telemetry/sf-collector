#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DRIVER_SRC_PATH=$(find /usr/src -name "falco-*")

cd $DIR && rm -rf build && mkdir -p build
cp -a $DIR/../../bin $DIR/build/bin
mkdir -p $DIR/build/driver && cp -a $DIR/../../modules/bin/falco-driver-loader $DIR/build/driver/.
mkdir -p $DIR/build/driver/src && cp -a $DRIVER_SRC_PATH $DIR/build/driver/src/$(basename $DRIVER_SRC_PATH)
cp -a $DIR/../service/systemd $DIR/build/service
cp $DIR/../service/bin/sysflow $DIR/build/bin/.
cp -a $DIR/../service/conf $DIR/build/conf
cp $DIR/../service/driver/start $DIR/build/driver/.
cp $DIR/../service/driver/cleanup $DIR/build/driver/.
cd $DIR

