#!/bin/bash
set -e
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
/usr/bin/falco-driver-loader bpf && $DIR/callback
