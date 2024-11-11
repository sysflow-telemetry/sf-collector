#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build-driver
DRIVER_SRC_PATH=$(find /usr/src -name "falco-*")

cd $DIR && rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
mkdir -p $DIR/$BUILD_DIR/bin && cp -a /usr/bin/falcoctl $DIR/$BUILD_DIR/bin/. && \
    cp -a /docker-entrypoint.sh $DIR/$BUILD_DIR/bin/.
mkdir -p $DIR/$BUILD_DIR/src && cp -a $DRIVER_SRC_PATH $DIR/$BUILD_DIR/src/$(basename $DRIVER_SRC_PATH) && \
    mkdir $DIR/$BUILD_DIR/src/dkms && cp -a $DIR/../../modules/src/dkms/*  $DIR/$BUILD_DIR/src/dkms

