#!/bin/bash
# Usage: build.sh version release target-image
target_img=$3
test_img="${target_img}-testing"
docker build --build-arg BUILD_NUMBER=$2 \
             --build-arg VERSION=$1 \
             --build-arg RELEASE=$2 \
             --target runtime \
             -t $target_img \
             .
docker build --cache-from $target_img --target testing -t $test_img .
