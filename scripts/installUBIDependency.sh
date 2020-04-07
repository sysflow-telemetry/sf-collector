
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
if [ "${MODE}" == "base" ] ; then
    # Run package installation for base images
    dnf install -y --disableplugin=subscription-manager http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-gpg-keys-8.1-1.1911.0.8.el8.noarch.rpm http://mirror.centos.org/centos/8/BaseOS/x86_64/os/Packages/centos-repos-8.1-1.1911.0.8.el8.x86_64.rpm
    dnf update -y --disableplugin=subscription-manager
    dnf install -y  --disableplugin=subscription-manager --disableexcludes=all --enablerepo=PowerTools \
            gcc \
            gcc-c++ \
            make \
            cmake \
            lua-devel \
            pkgconfig \
            autoconf \
            wget \
            automake \
            libtool \
            patch \
            binutils \
            bzip2 \
            perl \
            flex \
            bison \
            libstdc++-static \
            glibc-static \
            diffutils \
            kmod \
            epel-release \
            xz \
            boost-devel \
            elfutils-libelf-devel \
            apr-devel \
            apr-util-devel \
            sparsehash-devel \
            ncurses-devel \
            openssl-devel \
            glog-devel
    dnf update -y --disableplugin=subscription-manager
    dnf install -y --disableplugin=subscription-manager --disableexcludes=all --enablerepo=PowerTools dkms
    dnf -y clean all && rm -rf /var/cache/dnf
elif [ "${MODE}" == "test-extra" ] ; then
    # Install extra(compared to base) packages for tests
    dnf install -y --disableplugin=subscription-manager \
	    python3 \
        python3-wheel && \
    mkdir -p /usr/local/lib/python3.6/site-packages && \
    ln -s /usr/bin/easy_install-3 /usr/bin/easy_install && \
    ln -s /usr/bin/python3 /usr/bin/python && \
    ln -s /usr/bin/pip3 /usr/bin/pip && \
    dnf -y clean all && rm -rf /var/cache/dnf
else
    echo "Unsupported mode: ${MODE}"
fi

exit 0

