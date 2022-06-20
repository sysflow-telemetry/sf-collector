#!/bin/bash
INSTALL_PATH=/usr/local/sysflow
MODPREFIX=${INSTALL_PATH}/modules
FALCOPREFIX=/usr/
make SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER \
         LIBLOCALPREFIX=${FALCOPREFIX} \
         SDLOCALLIBPREFIX=/usr/lib/x86_64-linux-gnu/falcosecurity/ \
         SDLOCALINCPREFIX=/usr/include/falcosecurity/ \
         AVRLOCALLIBPREFIX=${MODPREFIX}/lib \
         AVRLOCALINCPREFIX=${MODPREFIX}/include \
         SFLOCALINCPREFIX=${MODPREFIX}/include/sysflow/c++ \
         FSLOCALINCPREFIX=${MODPREFIX}/include/filesystem \
         SCHLOCALPREFIX=${MODPREFIX}/conf \
         test