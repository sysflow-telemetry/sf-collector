#!/bin/bash
set -x
rm -fr /usr/src/kernels/ && rm -fr /usr/src/debug/
rm -fr /lib/modules && ln -s $SYSDIG_HOST_ROOT/lib/modules /lib/modules
rm -fr /boot && ln -s $SYSDIG_HOST_ROOT/boot /boot
if [ -S "/host/var/run/docker.sock" ] ; then
echo "Docker Engine domain socket detected"
fi
# if we found cri domain socket on one of CRI_PATH for CRIO or
# containerd.sock for containerd, link that to the real internal
# cri domain socket path we are using. Via this we can avoid 
# creating dummy links in local file system
if [ -S "/host${CRI_PATH}" ] ; then
echo "CRIO domain socket detected"
ln -s /host${CRI_PATH} /host${INTERNAL_CRI_PATH}
elif [ -S "/host/run/containerd/containerd.sock" ] ; then
echo "Containerd Domain Socket detected"
ln -s /host/run/containerd/containerd.sock /host${INTERNAL_CRI_PATH}
fi
exec /usr/local/sysflow/modules/bin/docker-entrypoint.sh "$@"
