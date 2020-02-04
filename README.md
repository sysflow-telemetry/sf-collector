[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector/builds)
[![Docker Pulls](https://img.shields.io/docker/pulls/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/sysflow-telemetry/sf-collector)
[![Documentation Status](https://readthedocs.org/projects/sysflow/badge/?version=latest)](https://sysflow.readthedocs.io/en/latest/?badge=latest)
[![GitHub](https://img.shields.io/github/license/sysflow-telemetry/sf-collector)](https://github.com/sysflow-telemetry/sf-collector/blob/master/LICENSE.md)

# Supported tags and respective `Dockerfile` links

-	[`0.1-rc3`](https://github.com/sysflow-telemetry/sf-collector/blob/0.1-rc3/Dockerfile), [`latest`](https://github.com/sysflow-telemetry/sf-collector/blob/master/Dockerfile)

# Quick reference

-	**Documentation**:  
	[the SysFlow Documentation](https://sysflow.readthedocs.io)
  
-	**Where to get help**:  
	[the SysFlow Community Slack](https://join.slack.com/t/sysflow-telemetry/shared_invite/enQtODA5OTA3NjE0MTAzLTlkMGJlZDQzYTc3MzhjMzUwNDExNmYyNWY0NWIwODNjYmRhYWEwNGU0ZmFkNGQ2NzVmYjYxMWFjYTM1MzA5YWQ)

-	**Where to file issues**:  
	[the github issue tracker](https://github.com/sysflow-telemetry/sf-docs/issues) (include the `sf-collector` tag)

-	**Source of this description**:  
	[repo's readme](https://github.com/sysflow-telemetry/sf-collector/edit/master/README.md) ([history](https://github.com/sysflow-telemetry/sf-collector/commits/master))

# What is SysFlow?

The SysFlow Telemetry Pipeline is a framework for monitoring cloud workloads and for creating performance and security analytics. The goal of this project is to build all the plumbing required for system telemetry so that users can focus on writing and sharing analytics on a scalable, common open-source platform. The backbone of the telemetry pipeline is a new data format called SysFlow, which lifts raw system event information into an abstraction that describes process behaviors, and their relationships with containers, files, and network. This object-relational format is highly compact, yet it provides broad visibility into container clouds. We have also built several APIs that allow users to process SysFlow with their favorite toolkits. Learn more about SysFlow in the [SysFlow specification document](https://sysflow.readthedocs.io/en/latest/spec.html).

# About This Image

This image packages SysFlow Collector, which monitors and collects system call and event information from hosts
and exports them in the SysFlow format using Apache Avro object serialization. The SysFlow Collector depends on [Sysdig](https://github.com/draios/sysdig) probe to passively collect system events and turn them into SysFlow. As a result, 
the collector supports Sysdig's powerful filtering capabilities. Please check the [Installation and Usage](https://sysflow.readthedocs.io/en/latest/build.html) for the complete set of options.

# How to use this image

### Starting the collection probe
The easiest way to run the SysFlow collector is from a Docker container, with host mount for the output trace files. The following command shows how to run sf-collector with trace files exported to `/mnt/data` on the host.

```
docker run -d --privileged --name sf-collector \
	     -v /var/run/docker.sock:/host/var/run/docker.sock \
	     -v /dev:/host/dev -v /proc:/host/proc:ro \
	     -v /boot:/host/boot:ro -v /lib/modules:/host/lib/modules:ro \
             -v /usr:/host/usr:ro -v /mnt/data:/mnt/data \
             -e INTERVAL=60 \
             -e EXPORTER_ID=${HOSTNAME} \
             -e OUTPUT=/mnt/data/    \
             -e FILTER="container.name!=sf-collector and container.name!=sf-exporter" \
             --rm sysflowtelemetry/sf-collector
```
where INTERVAL denotes the time in seconds before a new trace file is generated, EXPORTER\_ID sets the exporter name, OUTPUT is the directory in which trace files are written, and FILTER is the filter expression used to filter collected events. Note: append `container.type!=host` to FILTER expression to filter host events. 

Instructions for `docker compose` and `helm` deployments are available in [here](https://sysflow.readthedocs.io/en/latest/deploy.html).

### Inspecting collected traces
A [command line utilitiy](https://sysflow.readthedocs.io/en/latest/api-utils.html) is provided for inspecting collected traces or convert traces from SysFlow's compact binary format into human-readable JSON or CSV formats. 

```
docker run --rm -v /mnt/data:/mnt/data sysflowtelemetry/sysprint /mnt/data/<trace> 
```
where `trace` is the the name of the trace file inside `/mnt/data`. If empty, all files in `/mnt/data` are processed. By default, the traces are printed to 
standard output with a default set of SysFlow attributes. For a complete list of options, run:
```
docker run --rm -v /mnt/data:/mnt/data sysflowtelemetry/sysprint  -h
```

# License

View [license information](https://github.com/sysflow-telemetry/sf-collector/blob/master/LICENSE.md) for the software contained in this image.

As with all Docker images, these likely also contain other software which may be under other licenses (such as Bash, etc from the base distribution, along with any direct or indirect dependencies of the primary software being contained).

As for any pre-built image usage, it is the image user's responsibility to ensure that any use of this image complies with any relevant licenses for all software contained within.
