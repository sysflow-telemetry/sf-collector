subscription-manager repos --enable codeready-builder-for-rhel-9-$(arch)-rpms
dnf install \
https://dl.fedoraproject.org/pub/epel/epel-release-latest-9.noarch.rpm

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
        diffutils \
        kmod \
        zlib-devel \
        xz \
        apr-devel \
        apr-util-devel \
        openssl-devel \
        flex \
        bison \
        bc \
        libasan \
        libubsan \
        llvm-toolset \
        bpftool \
        libzstd-devel \
	pcre2-devel \
	boost-devel \
	libsepol-devel \
	sparsehash-devel \
	snappy-devel \
        elfutils-libelf-devel
