#!/bin/bash
# Usage: cleanup image
docker rmi $1-testing
docker rmi $1
docker image prune -f
docker rmi $(docker images -q sysflowtelemetry/ubi)
docker image prune -f
