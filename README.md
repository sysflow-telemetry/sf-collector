[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector/builds)
[![Docker Pulls](https://img.shields.io/docker/pulls/sysflowtelemetry/sf-collector)](https://hub.docker.com/r/sysflowtelemetry/sf-collector)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/sysflow-telemetry/sf-collector)
[![Documentation Status](https://readthedocs.org/projects/sysflow/badge/?version=latest)](https://sysflow.readthedocs.io/en/latest/?badge=latest)

# Quick reference

-	**Documentation**:  
	[the SysFlow Documentation](https://sysflow.readthedocs.io)
  
-	**Where to get help**:  
	[the SysFlow Community Slack](https://join.slack.com/t/sysflow-telemetry/shared_invite/enQtODA5OTA3NjE0MTAzLTlkMGJlZDQzYTc3MzhjMzUwNDExNmYyNWY0NWIwODNjYmRhYWEwNGU0ZmFkNGQ2NzVmYjYxMWFjYTM1MzA5YWQ)

-	**Where to file issues**:  
	[the github issue tracker](https://bugs.launchpad.net/cloud-images) (include the `sf-collector` tag)

-	**Source code of this image**:  
	[source repository](https://github.com/sysflow-telemetry/sf-collector) 

# What is SysFlow?

The SysFlow Telemetry Pipeline is a framework for monitoring cloud workloads and for creating performance and security analytics. The goal of this project is to build all the plumbing required for system telemetry so that users can focus on writing and sharing analytics on a scalable, common open-source platform. The backbone of the telemetry pipeline is a new data format called SysFlow, which lifts raw system event information into an abstraction that describes process behaviors, and their relationships with containers, files, and network. This object-relational format is highly compact, yet it provides broad visibility into container clouds. We have also built several APIs that allow users to process SysFlow with their favorite toolkits. Learn more about SysFlow in the [SysFlow specification document](https://sysflow.readthedocs.io/en/latest/spec.html).

# What's in this image?

This image packages the SysFlow Collector, which monitors and collects system call and event information from hosts
and exports them in the SysFlow format using Apache Avro object serialization. The SysFlow Collector depends on [Sysdig](https://github.com/draios/sysdig) probe to passively collect system events and turn them into SysFlow. As a result, 
the collector supports Sysdig's powerful filtering capabilities. Please check the [Installation and Usage](https://sysflow.readthedocs.io/en/latest/build.html) for complete set of options.

# License

View [license information](https://github.com/sysflow-telemetry/sf-collector/blob/master/LICENSE.md) for the software contained in this image.

As with all Docker images, these likely also contain other software which may be under other licenses (such as Bash, etc from the base distribution, along with any direct or indirect dependencies of the primary software being contained).

As for any pre-built image usage, it is the image user's responsibility to ensure that any use of this image complies with any relevant licenses for all software contained within.
