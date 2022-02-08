#!/bin/bash
set -e
subscription-manager repos --enable="codeready-builder-for-rhel-8-$(/bin/arch)-rpms"
dnf -y update
dnf -y install \
    gcc \
    make \
    kernel-devel-$(uname -r) \
    elfutils-libelf-devel \
    snappy-devel \
    glog-devel \
    llvm-toolset
dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
dnf -y install dkms
dnf -y remove epel-release && dnf autoremove
 
