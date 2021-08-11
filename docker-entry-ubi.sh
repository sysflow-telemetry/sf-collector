#!/bin/bash
set -x
rm -fr /usr/src/kernels/ && rm -fr /usr/src/debug/
rm -fr /lib/modules && ln -s $HOST_ROOT/lib/modules /lib/modules
rm -fr /boot && ln -s $HOST_ROOT/boot /boot
if [ -S "/host/var/run/docker.sock" ] ; then
echo "Docker Engine domain socket detected"
fi
echo "CRI_PATH: ${CRI_PATH}"
if [ ! -z "${CRI_PATH}" ] ; then
    echo "Adopt CRI_PATH: ${CRI_PATH}"
else
    # Auto-detecting the container runtime sockets when CRI_PATH is empty.
    # Also providing some information here about the environment
    numSocks=0
    if [ -S "/host/var/run/crio/crio.sock" ] ; then
        echo "CRIO domain socket detected"
        CRI_PATH="/var/run/crio/crio.sock"
        numSocks=$((numSocks+1))
    fi
    if [ -S "/host/run/containerd/containerd.sock" ] ; then
        echo "Containerd Domain Socket detected"
        CRI_PATH="/run/containerd/containerd.sock"
        numSocks=$((numSocks+1))
    fi
    if [ -S "/host/var/run/docker.sock" ] ; then
        echo "DOCKER domain socket detected"
        # Do not set CRI_PATH here, we will take DOCKER domain socket
        numSocks=$((numSocks+1))
    fi
    echo "${numSocks} container runtime sockets detected"
    if [ $numSocks -eq 0 ] ; then
        echo "WARNING: Unable to detect container runtime sockets. Will not be able to load container information"
    elif [ $numSocks -gt 1 ] ; then
        echo "WARNING: Multiple container runtime sockets found. This is not supported. Specify CRI_PATH environmental variable instead."
    fi
fi

exec /usr/local/sysflow/modules/bin/docker-entrypoint.sh "$@"    
