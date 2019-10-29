[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector/builds)
[![Docker Pulls](https://img.shields.io/docker/pulls/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/sysflow-telemetry/sf-collector)
[![Documentation Status](https://readthedocs.org/projects/sysflow/badge/?version=latest)](https://sysflow.readthedocs.io/en/latest/?badge=latest)

# SysFlow Collector (probe)

The SysFlow Collector monitors and collects system call and event information from hosts
and exports them in the SysFlow format using Apache Avro object serialization.  SysFlow lifts
system call information into a higher order object-relational format that models how containers,
processes, and files interact with their environment through process control flow, file, and network
operations. Learn more about SysFlow in the [SysFlow specification document](SPEC.md).

The SysFlow Collector is currently built upon the [Sysdig](https://github.com/draios/sysdig) core and 
requires the Sysdig probe to passively collect system events and turn them into SysFlow. As a result, 
the collector supports Sysdig's powerful filtering capabilities. Please see the [Installation and Usage](BUILD.md) for installing the collector.

* [Installation and Usage](BUILD.md)
* [SysFlow specification](SPEC.md)
* [License](LICENSE.md)
