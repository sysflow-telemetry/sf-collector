#!/bin/bash
# Usage: build.sh version release target-image
docker build --build-arg BUILD_NUMBER=$2 --build-arg VERSION=$1 --build-arg RELEASE=$2 --target runtime -t $3 .