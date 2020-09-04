#!/bin/bash
set -x
rm -fr /usr/src/kernels/ && rm -fr /usr/src/debug/
rm -fr /lib/modules && ln -s $SYSDIG_HOST_ROOT/lib/modules /lib/modules
rm -fr /boot && ln -s $SYSDIG_HOST_ROOT/boot /boot
if [ -S "/host/var/run/docker.sock" ] ; then
echo "Docker Engine domain socket detected"
fi
echo "CRI_PATH: ${CRI_PATH}"
if [ ! -z "${CRI_PATH}" ] ; then
    echo "Adopt CRI_PATH: ${CRI_PATH}"
elif [ -S "/host/var/run/docker.sock" ] ; then
    echo "DOCKER domain socket detected"
    # Do not set CRI_PATH here, we will take DOCKER domain socket
elif [ -S "/host/var/run/crio/crio.sock" ] ; then
    echo "CRIO domain socket detected"
    CRI_PATH="/var/run/crio/crio.sock"
elif [ -S "/host/run/containerd/containerd.sock" ] ; then
    echo "Containerd Domain Socket detected"
    CRI_PATH="/run/containerd/containerd.sock"
fi

exec /usr/local/sysflow/modules/bin/docker-entrypoint.sh ${DETECTED_CRI_PATH} "$@"    
