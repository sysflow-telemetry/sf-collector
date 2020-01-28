#!/bin/bash
rm -fr /lib/modules && ln -s $SYSDIG_HOST_ROOT/lib/modules /lib/modules
/usr/local/sysflow/modules/bin/docker-entrypoint.sh "$@"
