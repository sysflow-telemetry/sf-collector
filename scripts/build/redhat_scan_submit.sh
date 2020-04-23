#!/bin/bash
# Submit an image to RedHat Sysfow Exporter Jobs for certification
# Usage: redhat_scan_submit.sh [login secret] [image_to_be_scanned] [RedHat project ID] [container label]
set -e

docker login -u unused -p $1 scan.connect.redhat.com

docker tag $2 scan.connect.redhat.com/$3/test:$4
docker push scan.connect.redhat.com/$3/test:$4

docker rmi scan.connect.redhat.com/$3/test:$4


