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
    dnf -y clean all && rm -rf /var/cache/dnf
    subscription-manager unregister || true
}
trap cleanup EXIT

#
# RHEL subscription
#
(
    set +x
    if [ -z "$REGISTER_USER" -o -z "$REGISTER_PASSWORD" ] ; then
        echo 'Lack of RHEL credential.'
        echo 'Assume build on RHEL machines or install packages only in UBI repositories.'
    else
        echo "Login RHEL..."
        subscription-manager register --username "$REGISTER_USER" --password "$REGISTER_PASSWORD" --auto-attach
    fi
)

if [ "${MODE}" == "base" ] ; then
    # packages for base image
    
    subscription-manager repos --enable="codeready-builder-for-rhel-8-$(/bin/arch)-rpms"
    dnf -y install --disablerepo=epel \
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
        ncurses-devel \
        openssl-devel \
        flex \
        bison \
        libstdc++-static \
        boost-devel \
        elfutils-libelf-devel \
    	sparsehash-devel \
	    snappy-devel \
	    jsoncpp-devel \
	    glog-devel 

    # Install dkms from EPEL.
    # ref: https://access.redhat.com/solutions/1132653
    dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
    dnf -y install dkms
    dnf -y remove epel-release

elif [ "${MODE}" == "test-extra" ] ; then
    # additional packages for testing

    dnf -y install python3 python3-devel python3-wheel
    mkdir -p /usr/local/lib/python3.6/site-packages
    ln -s /usr/bin/easy_install-3 /usr/bin/easy_install
    ln -s /usr/bin/python3 /usr/bin/python
    ln -s /usr/bin/pip3 /usr/bin/pip

else
    echo "Unsupported mode: ${MODE}"
    exit 1
fi

exit 0
