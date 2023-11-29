#
# Copyright (C) 2022 IBM Corporation.
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

ARG FALCO_LIBS_VER
ARG FALCO_VER
ARG UBI_VER

#-----------------------
# Stage: Builder
#-----------------------
FROM sysflowtelemetry/ubi:base-${FALCO_LIBS_VER}-${FALCO_VER}-${UBI_VER} AS builder

# sysflow release
ARG SYSFLOW_VERSION

# install libsysflow
RUN wget https://github.com/sysflow-telemetry/sf-collector/releases/download/${SYSFLOW_VERSION}/libsysflow-${SYSFLOW_VERSION}-x86_64.tar.gz && \
    tar xzf libsysflow-${SYSFLOW_VERSION}-x86_64.tar.gz && cp -r libsysflow-${SYSFLOW_VERSION}-x86_64/usr/* /usr/.

# build the example program
COPY callback.cpp /build/
COPY Makefile /build/
COPY manifest /build/
RUN make -C /build install

#-----------------------
# Stage: Runtime
#-----------------------
FROM sysflowtelemetry/ubi:driver-${FALCO_LIBS_VER}-${FALCO_VER}-${UBI_VER} AS runtime

COPY --from=builder /usr/local/example/bin/callback /usr/local/example/bin/callback

CMD ["/usr/local/example/bin/callback"]

