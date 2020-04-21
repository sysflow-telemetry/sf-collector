#!/bin/bash
# Submit an image to RedHat Sysfow Exporter Jobs for certification
# Usage: test.sh ci-image
docker build --cache-from $1 --target testing -t $1-testing .
docker run --rm --name sftests -v "$(pwd)/tests:/usr/local/sysflow/tests" $1-testing -t tests/tests.bats
