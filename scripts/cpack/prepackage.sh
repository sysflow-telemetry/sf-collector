#!/bin/sh
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $DIR && rm -rf build && mkdir -p build
cp -a $DIR/../../bin $DIR/build/bin
cp -a $DIR/../../conf $DIR/build/conf
cd $DIR

