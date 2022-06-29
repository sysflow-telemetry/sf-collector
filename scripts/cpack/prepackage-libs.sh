#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build-libs

cd $DIR && rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
mkdir -p $DIR/$BUILD_DIR/lib && cp -a /usr/lib/x86_64-linux-gnu/falcosecurity $DIR/$BUILD_DIR/lib/.
mkdir -p $DIR/$BUILD_DIR/include && cp -a /usr/include/falcosecurity $DIR/$BUILD_DIR/include/.
mkdir -p $DIR/$BUILD_DIR/lib && cp -a /usr/local/sysflow/lib $DIR/$BUILD_DIR/lib/sysflow
mkdir -p $DIR/$BUILD_DIR/include && cp -a /usr/local/sysflow/include/sysflow $DIR/$BUILD_DIR/include/.
cp $DIR/../../LICENSE.md $DIR/$BUILD_DIR/.
cp $DIR/../../README.md $DIR/$BUILD_DIR/.
cd $DIR

