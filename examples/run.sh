#!/bin/bash
set -e
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export DRIVER_NAME=falco
export FALCO_BPF_PROBE=""
export DRIVERS_REPO=https://download.falco.org/driver
/usr/bin/falco-driver-loader bpf && $DIR/callback e
