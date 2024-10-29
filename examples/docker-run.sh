#!/bin/bash
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export $(grep -v '^#' $DIR/manifest | xargs | sed 's/?//g')
docker run \
    --privileged \
    -v /var/run/docker.sock:/host/var/run/docker.sock \
    -v /dev:/host/dev \
    -v /proc:/host/proc:ro \
    -v /boot:/host/boot:ro \
    -v /lib/modules:/host/lib/modules:ro \
    -v /usr:/host/usr:ro \
    -v /etc/:/host/etc:ro \
    -v /var/lib:/host/var/lib:ro \
    -e FALCO_DRIVER_LOADER_OPTIONS="ebpf" \
    -e DRIVER_TYPE="e" \
    --rm callback:${SYSFLOW_VERSION}
