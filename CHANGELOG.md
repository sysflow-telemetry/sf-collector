# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

> **Types of changes:**
>
> - **Added**: for new features.
> - **Changed**: for changes in existing functionality.
> - **Deprecated**: for soon-to-be removed features.
> - **Removed**: for now removed features.
> - **Fixed**: for any bug fixes.
> - **Security**: in case of vulnerabilities.

## [Unreleased]

## [0.5.0] - 2022-07-31

### Added

- Add initial version of libsysflow
- Add example consumer of libsysflow

### Changed

- Build pipeline using sysflow-telemetry/libs-base-images

## [0.4.3] - 2022-06-21

### Changed

- Add plugin dir configuration to systemd service

## [0.4.2] - 2022-06-13

### Changed

- Bumped SysFlow version to 0.4.2

## [0.4.1] - 2022-05-26

### Changed

- Bumped UBI version to 8.6-754
- Compiles from source and links libelf, glog, and snappy statically to sysporter
- Packages dkms sources and installation script as part of binary packages
- Removed binary package requirements from installers
- Add DRIVER_OPTS binary packages configuration environment for the driver loader
- Add optimization options in binary package configuration
- Update default configuration settings for binary package

## [0.4.0] - 2022-02-18

### Added

- Packaging in deb, rpm, and targz formats

### Changed

- Updated to use falco-libs and falco pre-0.31.0
- Compilies avro and json statically into syspoter
- Updated collector usage string to document -u and -w flags
- Collector now tries to reconnect when the processor socket connection is severed
- Updated CI to automate packaging or release assets with release notes
- Bumped UBI version to 8.5-226.

### Fixed

- Removed header imports causing a conflict between validate-json and json libraries built by falco libs
- Cleanup module paths and makefiles

### Removed

- Remove build dependency on sysdig (uses falco libs now)

## [0.3.1] - 2021-09-29

### Changed

- Bumped UBI version to 8.4-211.

## [0.3.0] - 2021-09-20

### Added

- Trace file information to SysFlow header object
- Concurrent export of SysFlow traces to socket and file

### Changed

- Moved away from Dockerhub CI.
- Upgraded to sysdig 0.27.1
- Tracking latest sysflow APIs.

## [0.2.2] - 2020-12-07

### Changed

- Upgraded system packages in base image
- Tracking latest sysflow APIs.

## [0.2.1] - 2020-12-02

### Changed

- Tracking latest sysflow APIs.

## [0.2.0] - 2020-12-01

### Added

- Added optional enabling of ProcessFlow objects through `ENABLE_PROC_FLOW=1`.
- Added enablement of sysdig syscall drop mode support through `ENABLE_DROP_MODE=1`.
- Added ability to disable non-file related FileFlows with `FILE_ONLY=1`.
- Added ability to disable, limit file read based fileflows with `FILE_READ_MODE=`.
    - `0` enable all file reads.
    - `1` disable all file reads.
    - `2` disable file reads to: `"/proc/", "/dev/", "/sys/", "//sys/", "/lib/", "/lib64/", "/usr/lib/", "/usr/lib64/"`
- Added LLVM to support eBPF probes in future release.
- Added Non-RHEL dependent developer build based on centos packages.
- Added debug Dockerfile for performance profiling with google-perf-tools.

### Changed

- Port to sysdig probe 0.27.0.
- `sysporter` now compiled with optimization 3.

### Fixed

- Fixed parenting issue for multi-thread applications.
- Fixed performance bottlenecks around utils::getExportTime function.
- Fixed command line parsing issue on PPC and Z platforms.

## [0.1.0] - 2020-10-30

### Added

- Added support for containerd; container runtime auto-detection.

### Changed

- Removed dependency to ncurses.

## [0.1.0-rc4] - 2020-08-10

### Added

- Added node IP field to the header.
- Added entry field to the process object.
- Implemented ProcessFlow object.
- Added mmap support for files.
- Add labels for container images.
- Embed license file inside the container image.

### Changed

- New Avro schema (version 2).
- Increased the nf/ff expire time to 60 seconds.
- Port to sysdig probe 0.26.7.
- Increased `sf-collector` version to the latest release candidate 0.1.0-rc4.
- Adding patch level to comply with semantic versioning.

### Fixed

- Fixed SIGTERM bug.
- Fixed container context to prevent a sysporter crash due to a null variable.
- Fixed memory leak found in sysdig 0.26.4 [CRITICAL].

## [0.1-rc3] - 2020-03-17

### Added

- Added domain sockets as additional output to the collector..

### Changed

- Ported base image to ubi/ubi8.
- Split the base/mods images from runtime image to speedup CI cycles.
- Changed logger to use glog.
- Updated filesystem module to version v1.2.10 (for computing canonical paths).
- Increased `sf-collector` version to the latest release candidate 0.1-rc3.

### Fixed

- Fixed ppid issue with children missing parent processes that terminate before a child is spawned.
- Fixed issue [12](https://github.com/sysflow-telemetry/sf-docs/issues/12).
- Fixed issue [13](https://github.com/sysflow-telemetry/sf-docs/issues/13).
- Fixed corrupt memory error that caused coredump when using domain socket interface.

## [0.1-rc2] - 2019-11-08

### Changed

- Port to Sysdig probe 0.26.4.
- Increased `sf-collector` version to the latest release candidate 0.1-rc2.

### Added

- Added script for debugging collector with Valgrind.

## [0.1-rc1] - 2019-10-31

### Added

- First release candidate of SysFlow Collector.

[Unreleased]: https://github.com/sysflow-telemetry/sf-collector/compare/0.4.3...HEAD
[0.5.0]: https://github.com/sysflow-telemetry/sf-collector/compare/0.4.3...0.5.0
[0.4.3]: https://github.com/sysflow-telemetry/sf-collector/compare/0.4.2...0.4.3
[0.4.2]: https://github.com/sysflow-telemetry/sf-collector/compare/0.4.1...0.4.2
[0.4.1]: https://github.com/sysflow-telemetry/sf-collector/compare/0.4.0...0.4.1
[0.4.0]: https://github.com/sysflow-telemetry/sf-collector/compare/0.3.1...0.4.0
[0.3.1]: https://github.com/sysflow-telemetry/sf-collector/compare/0.3.0...0.3.1
[0.3.0]: https://github.com/sysflow-telemetry/sf-collector/compare/0.2.2...0.3.0
[0.2.2]: https://github.com/sysflow-telemetry/sf-collector/compare/0.2.1...0.2.2
[0.2.1]: https://github.com/sysflow-telemetry/sf-collector/compare/0.2.0...0.2.1
[0.2.0]: https://github.com/sysflow-telemetry/sf-collector/compare/0.1.0...0.2.0
[0.1.0]: https://github.com/sysflow-telemetry/sf-collector/compare/0.1.0-rc4...0.1.0
[0.1.0-rc4]: https://github.com/sysflow-telemetry/sf-collector/compare/0.1-rc3...0.1.0-rc4
[0.1-rc3]: https://github.com/sysflow-telemetry/sf-collector/compare/0.1-rc2...0.1-rc3
[0.1-rc2]: https://github.com/sysflow-telemetry/sf-collector/compare/0.1-rc1...0.1-rc2
[0.1-rc1]: https://github.com/sysflow-telemetry/sf-collector/releases/tag/0.1-rc1
