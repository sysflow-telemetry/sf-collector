#!/usr/bin/env bats
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
#

TDIR=${WDIR}/tests
sfcomp=${TDIR}/sffilecomp.py
sysporter=${WDIR}/bin/sysporter
exporter=tests

@test "Trace comparison on TCP client server communication" {
  skip "Failing after upgrade to libs 0.18.1. Pending investigation. Skipping for this release."
  tdir=${TDIR}/client-server
  tfile=tcp-client-server
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on file operations" {
  skip "Failing after upgrade to libs 0.18.1. Pending investigation. Skipping for this release."
  tdir=${TDIR}/files
  tfile=files
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on file 'at' operations" {
  skip "Failing after upgrade to libs 0.18.1. Pending investigation. Skipping for this release."
  tdir=${TDIR}/files
  tfile=filesat
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on event mpm web server (cold start)" {
  tdir=${TDIR}/mpm-event
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on event mpm web server" {
  skip "Fixed in libs 0.14.x; it will be fixed in sysflow 0.8.0"
  tdir=${TDIR}/mpm-event
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > ${tdir}/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on prefork mpm web server (cold start)" {
  tdir=${TDIR}/mpm-preforked
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on prefork mpm web server" {
  skip "Fixed in libs 0.14.x; it will be fixed in sysflow 0.8.0"
  tdir=${TDIR}/mpm-preforked
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on multi-threaded mpm web server (cold start)" {
  tdir=${TDIR}/mpm-worker
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on multi-threaded mpm web server" {
  skip "Fixed in libs 0.14.x; it will be fixed in sysflow 0.8.0"
  tdir=${TDIR}/mpm-worker
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on namespace operations" {
  tdir=${TDIR}/namespaces
  tfile=setns
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store listing operations" {
  tdir=${TDIR}/objectstore
  tfile=ls
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store upload operations" {
  tdir=${TDIR}/objectstore
  tfile=upload
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store download operations" {
  tdir=${TDIR}/objectstore
  tfile=download
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on setuid" {
  tdir=${TDIR}/setuid
  tfile=setuid
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on alpine" {
  tdir=${TDIR}/alpine
  tfile=alpine
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on rename operations" {
  tdir=${TDIR}/rename
  tfile=rename
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}

@test "Trace comparison on rename2_at operations" {
  skip "Failing after upgrade to libs 0.18.1. Pending investigation. Skipping for this release."
  tdir=${TDIR}/rename
  tfile=rename2at
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  if [ $quiet ]; then
      run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  else
      $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf >&3
  fi
  [ ${status} -eq 0 ]
}
