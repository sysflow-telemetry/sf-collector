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

if [ "${MODE}" == "base" ] ; then
     # packages for base image
     dnf install -y --disableplugin=subscription-manager http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-gpg-keys-8.2-2.2004.0.1.el8.noarch.rpm http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-repos-8.2-2.2004.0.1.el8.x86_64.rpm
     dnf install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
     dnf -y install --enablerepo=PowerTools --disablerepo=ubi-8-codeready-builder  --disablerepo=ubi-8-appstream --disablerepo=ubi-8-baseos --disableplugin=subscription-manager \
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

     dnf install -y --disableplugin=subscription-manager --disableexcludes=all --enablerepo=epel --disablerepo=ubi-8-codeready-builder  --disablerepo=ubi-8-appstream --disablerepo=ubi-8-baseos  dkms
     dnf -y clean all && rm -rf /var/cache/dnf

 elif [ "${MODE}" == "test-extra" ] ; then
    # additional packages for testing
    dnf install -y --disableplugin=subscription-manager \
	    python3 \
        python3-devel \
        python3-wheel && \
    mkdir -p /usr/local/lib/python3.6/site-packages && \
    ln -s /usr/bin/easy_install-3 /usr/bin/easy_install && \
    ln -s /usr/bin/python3 /usr/bin/python && \
    ln -s /usr/bin/pip3 /usr/bin/pip && \
    dnf -y clean all && rm -rf /var/cache/dnf

else
    echo "Unsupported mode: ${MODE}"
    exit 1
fi

exit 0
