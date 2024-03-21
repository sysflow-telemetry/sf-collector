MAKE_JOBS=8
INSTALL_PATH=/usr/local/sysflow/
cd modules && make INSTALL_PATH=${INSTALL_PATH} MAKE_JOBS=${MAKE_JOBS} install
