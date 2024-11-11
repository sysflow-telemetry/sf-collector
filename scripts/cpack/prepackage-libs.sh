#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR=build-libs

cd $DIR && rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
mkdir -p $DIR/$BUILD_DIR/etc/falco && touch $DIR/$BUILD_DIR/etc/falco/falco.yaml
mkdir -p $DIR/$BUILD_DIR/lib && cp -a /usr/local/sysflow/modules/lib/falcosecurity $DIR/$BUILD_DIR/lib/.
mkdir -p $DIR/$BUILD_DIR/include && cp -a /usr/local/sysflow/modules/include/falcosecurity $DIR/$BUILD_DIR/include/.
mkdir -p $DIR/$BUILD_DIR/lib && cp -a /usr/local/sysflow/lib $DIR/$BUILD_DIR/lib/sysflow && \
    cp -a /usr/local/sysflow/modules/lib/*.a $DIR/$BUILD_DIR/lib/sysflow/. && \
    cp -a /usr/local/lib64/libsnappy.a $DIR/$BUILD_DIR/lib/sysflow/. && \
    cp -a /usr/local/lib64/libglog.a $DIR/$BUILD_DIR/lib/sysflow/.
mkdir -p $DIR/$BUILD_DIR/include && cp -a /usr/local/sysflow/include/sysflow $DIR/$BUILD_DIR/include/. && \
    cp -a /usr/local/sysflow/modules/include/sysflow/c++/sysflow/* $DIR/$BUILD_DIR/include/sysflow/. && \
    cp -a /usr/local/sysflow/modules/include/avro $DIR/$BUILD_DIR/include/sysflow/. && \
    cp -a /usr/local/sysflow/modules/include/filesystem $DIR/$BUILD_DIR/include/sysflow/. && \
    cp -a /usr/local/include $DIR/$BUILD_DIR/include/sysflow/mods
cp $DIR/../../LICENSE.md $DIR/$BUILD_DIR/.
cp $DIR/../../README.md $DIR/$BUILD_DIR/.

# strip static libraries
find "$BUILD_DIR" -type f -name "*.a" -exec strip -g '{}' \;

