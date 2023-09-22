#!/bin/bash
#
# Copyright (C) 2019 IBM Corporation.
#
# Authors:
# Frederico Araujo <frederico.araujo@ibm.com>
# Teryl Taylor <terylt@ibm.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# dependencies

# Insatall required dependencies to the UBI base image.
# Based on different purpose, we will install different stuff inside
# Mode we supports: base and test-extra
set -ex
MODE=${1:-base}

echo "Install Dependency under mode: ${MODE}"

#
# Clean up function
#
cleanup() {
    dnf -y clean all
    rm -rf /var/cache/{dnf,yum} || true
    subscription-manager unregister || true
    dnf -y remove \
        python3-subscription-manager-rhsm \
        subscription-manager \
        subscription-manager-rhsm-certificates \
        vim-minimal \
        || true
}
trap cleanup EXIT

#
# RHEL subscription
#
(
    set +x
    which subscription-manager || dnf -y install subscription-manager
    if [ -z "$REGISTER_USER" -o -z "$REGISTER_PASSWORD" ] ; then
        echo 'Lack of RHEL credential.'
        echo 'Assume build on RHEL machines or install packages only in UBI repositories.'
    else
        echo "Login RHEL..."
        # See https://access.redhat.com/discussions/5889431?tour=8
        sed -i 's/\(def in_container():\)/\1\n    return False/g' /usr/lib64/python*/*-packages/rhsm/config.py
        subscription-manager register --username "$REGISTER_USER" --password "$REGISTER_PASSWORD" --auto-attach
    fi
)

if [ "${MODE}" == "base" ] ; then
    # packages for base image

    subscription-manager repos --enable="codeready-builder-for-rhel-9-$(/bin/arch)-rpms" && \
    dnf -y update && \
    dnf -y install \
        gcc \
        gcc-c++ \
        make \
        cmake \
        pkgconfig \
        autoconf \
        gettext-devel \
        wget \
        automake \
        libtool \
        patch \
        binutils \
        bzip2 \
        perl \
        glibc-static \
        diffutils \
        kmod \
        zlib-devel \
        xz \
        apr-devel \
        apr-util-devel \
        openssl-devel \
        flex \
        bison \
        libstdc++-static \
        boost-static \
        bc \
        libasan \
        libubsan \
        llvm-toolset \
        bpftool \
    && dnf -y clean all ; rm -rf /var/cache/{dnf,yum}


elif [ "${MODE}" == "driver" ] ; then
    # packages for driver image

    subscription-manager repos --enable="codeready-builder-for-rhel-9-$(/bin/arch)-rpms" && \
    dnf -y update && \
    dnf -y install \
        gcc \
        gcc-c++ \
        gcc-toolset-12-gcc \
        make \
        cmake \
        pkgconfig \
        autoconf \
        wget \
        automake \
        libtool \
        patch \
        binutils \
        bzip2 \
        perl \
        glibc-static \
        diffutils \
        kmod \
        xz \
        openssl-devel \
        flex \
        bison \
        libstdc++-static \
        bc \
        libasan \
        libubsan \
        llvm-toolset \
        elfutils-libelf-devel \
    && dnf -y clean all ; rm -rf /var/cache/{dnf,yum}

elif [ "${MODE}" == "test-extra" ] ; then
    # additional packages for testing

    dnf -y --noplugins install python38 python38-devel python38-wheel
    ln -s /usr/bin/python3 /usr/bin/python
    mkdir -p /usr/local/lib/python3.8/site-packages

else
    echo "Unsupported mode: ${MODE}"
    exit 1
fi

exit 0
