#!/bin/bash
# Usage: build.sh version release falcover libsver ubiver target
target=$6
docker build --build-arg BUILD_NUMBER=$2 \
             --build-arg VERSION=$1 \
             --build-arg RELEASE=$2 \
             --build-arg FALCO_VER=$3 \
             --build-arg FALCO_LIBS_VER=$4 \
             --build-arg UBI_VER=$5 \
             --target $target \
             -t sf-collector:$target \
             .
docker build --cache-from=sf-collector:$target --target=testing -t sf-collector:testing .
