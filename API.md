#  SysFlow APIs

SysFlow uses Apache Avro (https://avro.apache.org/) serialization to output data.    We used Avro to be able to create compact records that can be processed by a wide variety of programming languages, and big data analytics platforms such as Apache Spark (https://spark.apache.org/).    Avro enables a user to generate programming stubs for serializing and deserializing data, using either Apache Avro IDL (https://avro.apache.org/docs/1.8.2/idl.html) or Apache schema files (https://avro.apache.org/docs/1.8.2/spec.html).  

## Avro IDL and Schema Files

The Avro IDL files for SysFlow are available in the repository under <code>sf-collector/src/avro/avdl</code>, while the schema files are available under <code>sf-collector/src/avro/avsc</code>.   The `avrogen` tool can be used to generate classes using the schema.  See <code>sf-collector/src/avro/generateCClasses.sh</code> for an example of how to generate C++ headers from apache schema files.  

##  SysFlow Avro C++

SysFlow C++ SysFlow objects and encoders/decoders are all available in <code>sf-collector/src/avro/src/sysflow/sysflow.hh</code>.  <code>sf-collector/src/avro/src/sysreader.cpp</code> provides a good example of how to read and process different SysFlow avro objects in C++.   <code>sysreader</code> is compiled by default using Makefile in <code>sf-collector/src/</code>.   Note, one must install Apache Avro 1.8.2 cpp (https://avro.apache.org/releases.html) to run an application that includes sysflow.hh.  The library file   <code>-lavrocpp</code> must also be linked during compilation. 

## SysFlow Avro Python 3.0

We have also provided SysFlow python 3.0 API's that were generated with the avro-gen python package.    These classes are available in <code>sf-collector/src/avro/py3</code>.

In order to install the SysFlow python package, do the following:

<code>cd sf-collector/src/avro/py3</code><br/>
<code>sudo python3 setup.py install</code>

