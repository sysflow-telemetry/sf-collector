#!/bin/bash
rm -fr /lib/modules && ln -s $SYSDIG_HOST_ROOT/lib/modules /lib/modules
rm -fr /boot && ln -s $SYSDIG_HOST_ROOT/boot /boot
/usr/local/sysflow/modules/bin/docker-entrypoint.sh "$@"
