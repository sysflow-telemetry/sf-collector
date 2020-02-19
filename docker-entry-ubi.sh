#!/bin/bash
rm -fr /usr/src/kernels/ && rm -fr /usr/src/debug/
rm -fr /lib/modules && ln -s $SYSDIG_HOST_ROOT/lib/modules /lib/modules
rm -fr /boot && ln -s $SYSDIG_HOST_ROOT/boot /boot
/usr/local/sysflow/modules/bin/docker-entrypoint.sh "$@"
