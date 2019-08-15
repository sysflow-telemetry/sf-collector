# Build, Installation and Usage

## Cloning source

The sf-collector project has been tested primarily on ubuntu 16.04 and 18.04.  The project should build and run on other flavors of UNIX and will be tested in the future. Building and running the application inside a docker container is the easiest way to run it, but this document will describe how to build and run the application in both docker and on a linux host.

To build the project, first pull down the source code, with submodules:
<br><br>
<code>git clone git@github.ibm.com:sysmon/sf-collector.git --recursive</code>

To checkout submodules on an already cloned repo:
<br><br>
<code>git submodule update --init --recursive</code>

## Build Docker container

To build the docker container: 
<br><br>
<code> cd sf-collector </code>
<br>
<code> ./build </code>

The container is built in stages to enable caching of the intermediate steps of the build.  As a result, the docker container of interest is: <code>sysporter:runtime</code>

## Build directly on host

First, the dependencies to build sysdig and the avro libraries must be installed:


<code> apt-utils build-essential libncurses5-dev libncursesw5-dev cmake libboost-all-dev flex bison g++ wget libelf-dev</code>

Then install the dependencies for sysporter.  Note, it is recommended that sysporter is built with g++8 or above.  If building with older g++ versions, one must install the libboost filesystem library that supports the <code>weakly_canonical</code> API:

<code>apt-utils make libboost-all-dev g++-8 libelf-dev liblog4cxx-dev libapr1 libaprutil1 libsparsehash-dev</code>

<code>ln -s /usr/bin/g++-8 /usr/bin/g++</code>

<code>make install</code>

## Running sysporter

### Running sysporter from the command line 

Sysporter has the following options:
<br><br>
<code>Usage: ./sysporter [-h] [-G &lt;interval&gt;] [-s &lt;schema file&gt;] [-c] [-e &lt;exporterID&gt;] [-r &lt;scap file&gt;] [-l &lt;log conf file&gt;] -w &lt;file name/dir&gt;</code>

<code>Options:</code>


| Option| Description|    
|---------------------|-------------------------------|
|-h|Show this help message.|
|-w &lt;file name/dir&gt;|(required) The file or directory to which sysflow records are written. If a directory is specified (using a trailing slash), file name will be an epoch timestamp. If -G is specified, then the file name specified will have an epoch timestamp appended to it.|
|-e &lt;exporterID&gt;| A globally unique ID representing the host or VM being monitored which is stored in the sysflow dumpfile header.  If -e not set, the hostname of the CURRENT machine is used, which may not be accurate for reading offline scap files.|
|-G &lt;interval(in secs)&gt;| Rotates the dumpfile specified in -w every interval seconds and appends epoch timestamp to file name.|
|-r &lt;scap file&gt;|The scap file to be read and dumped as sysflow format at the file specified by -w.  If this option is not specified, a live capture is assumed.|
|-s &lt;schema file&gt;|The sysflow avro schema file (.avsc) used for schema validation (default: /usr/local/sysflow/conf/SysFlow.avsc)|
|-f &lt;filter&gt;| Sysdig style filtering string to filter scap. Must be surrounded by quotes.|
|-c|Simple, fast filter to allow only container-related events to be dumped.|
|-l &lt;log conf file&gt;|Location of log4cxx properties configuration file. (default: /usr/local/sysflow/conf/log4cxx.properties). Properties file follows log4j format. See conf directory in github for example.  Setting log level to debug is extremely verbose.|
|-v|Prints the version of ./sysporter and exits.|
 
### Example Usage

Convert sysdig scap file to sysflow with export id.   Output will be written to output.sf.  Note that sysporter must be run with root privileges:

<code>./sysporter -r input.scap -w ./output.sf  -e host </code>

Trace a system live, and output sysflow to files in a directory which are rotated every 30 seconds. The file names with be an epoch timestamp of when the file was initially written.  Note, trailing slash must be present. The filter ensures that only sysflow from containers is generated.

<code>./sysporter -G 30 -w ./output/ -e host -f "container.type!=host and container.type=docker" </code>

Trace a system live, and output sysflow to files in a directory which are rotated every 30 seconds. The file names with be an output.<epoch timestamp> where the timestamp is of when the file was initially written.   The filter ensures that only sysflow from containers is generated.

<code>./sysporter -G 30 -w ./output/output -e host -f "container.type!=host and container.type=docker" </code>

## Running sysporter from the docker container

The easiest way to run sysporter is from the docker container, with host mount for the files.  The following command demonstrates how to run sysporter where files are exported to <code>/mnt/data</code> on the host.

<code>sudo mkdir -p /mnt/data</code>

<code>sudo docker pull floripa.sl.cloud9.ibm.com/sf-collector:latest</code>

<pre> sudo docker run -d --privileged --name sf-collector  -v /var/run/docker.sock:/host/var/run/docker.sock \
             -v /dev:/host/dev -v /proc:/host/proc:ro -v /boot:/host/boot:ro -v /lib/modules:/host/lib/modules:ro \
             -v /usr:/host/usr:ro -v /mnt/data:/mnt/data \
             -e INTERVAL=300 \
             -e NODE_NAME="${NODE}" \
             -e OUTPUT=/mnt/data    \
             -e FILTER="-f \"container.type!=host and container.type=docker and container.name!=sf-collector and not (container.name contains sf-exporter)\"" \
             --rm -i -t  floripa.sl.cloud9.ibm.com/sf-collector
</pre>

Where INTERVAL corresponds to the value of <code>-G</code> in the sysporter, NODE_NAME corresponds to the value of <code>-e</code>, OUTPUT corresponds to the value of <code>-w</code>, and FILTER represents both the flag (<code>-f</code>) and actual filter of the sysporter (see above).
