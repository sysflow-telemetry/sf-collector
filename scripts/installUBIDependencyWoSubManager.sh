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

# Install required dependencies to the UBI base image.
# Based on mode, it installs different set of dependencies.
# Modes supported: base and test-extra.
set -ex
MODE=${1:-base}

echo "Install Dependency under mode: ${MODE}"

#
# Clean up function
#
cleanup() {
    dnf -y clean all && rm -rf /var/cache/dnf
}
trap cleanup EXIT

if [ "${MODE}" == "base" ] ; then
    # packages for base image
    dnf -y install --disableplugin=subscription-manager \
        http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-gpg-keys-8.2-2.2004.0.1.el8.noarch.rpm http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-repos-8.2-2.2004.0.1.el8.x86_64.rpm
    dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
    dnf -y install \
        --disableplugin=subscription-manager \
        --disablerepo=ubi-8-appstream \
        --disablerepo=ubi-8-baseos \
        --disablerepo=ubi-8-codeready-builder \
        --enablerepo=PowerTools \
        gcc \
        gcc-c++ \
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
        apr-devel \
        apr-util-devel \
        openssl-devel \
        flex \
        bison \
        libstdc++-static \
        boost-devel \
        elfutils-libelf-devel \
        sparsehash-devel \
        snappy-devel \
        jsoncpp-devel \
        glog-devel \
        llvm-toolset

    dnf -y install \
        --disableexcludes=all \
        --disableplugin=subscription-manager \
        --disablerepo=ubi-8-appstream \
        --disablerepo=ubi-8-baseos \
        --disablerepo=ubi-8-codeready-builder \
        --enablerepo=epel \
        dkms

elif [ "${MODE}" == "test-extra" ] ; then
    # additional packages for testing

    dnf -y --noplugins install python38 python38-devel python38-wheel
    mkdir -p /usr/local/lib/python3.8/site-packages

else
    echo "Unsupported mode: ${MODE}"
    exit 1
fi

exit 0
