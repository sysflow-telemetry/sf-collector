#!/bin/bash
set -e
/usr/bin/falco-driver-loader -bpf && callback 
