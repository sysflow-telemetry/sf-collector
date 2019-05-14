#!/usr/bin/env bats

TDIR=${WDIR}/tests
sfcomp=${TDIR}/sffilecomp.py
sysporter=${WDIR}/bin/sysporter
exporter=tests

@test "Trace comparison on TCP client server communication" {
  tdir=${TDIR}/client-server
  tfile=tcp-client-server
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on file operations" {
  tdir=${TDIR}/files
  tfile=files
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on file 'at' operations" {
  tdir=${TDIR}/files
  tfile=filesat
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on event mpm web server (cold start)" {
  tdir=${TDIR}/mpm-event
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on event mpm web server" {
  tdir=${TDIR}/mpm-event
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on prefork mpm web server (cold start)" {
  tdir=${TDIR}/mpm-preforked
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on prefork mpm web server" {
  tdir=${TDIR}/mpm-preforked
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on multi-threaded mpm web server (cold start)" {
  tdir=${TDIR}/mpm-worker
  tfile=cold_start_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on multi-threaded mpm web server" {
  tdir=${TDIR}/mpm-worker
  tfile=full_capture
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on namespace operations" {
  tdir=${TDIR}/namespaces
  tfile=setns
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store listing operations" {
  tdir=${TDIR}/objectstore
  tfile=ls
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store upload operations" {
  tdir=${TDIR}/objectstore
  tfile=upload
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on object store download operations" {
  tdir=${TDIR}/objectstore
  tfile=download
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}

@test "Trace comparison on setuid" {
  tdir=${TDIR}/setuid
  tfile=setuid
  run $sysporter -r ${tdir}/${tfile}.scap -w /tmp/${tfile}.sf -e $exporter > /tmp/${tfile}.log
  run $sfcomp /tmp/${tfile}.sf ${tdir}/${tfile}.sf
  [ ${status} -eq 0 ]
}
