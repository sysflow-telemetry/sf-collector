import avro.io
import io
from sysflow import SpecificDatumReader as SysFlowReader 
from avro import datafile, io 
from  sysflow.schema_classes import SCHEMA as SysFlowSchema
from sysflow.sysflow import SysFlow
from avro import datafile, io 
from uuid import UUID
class SysflowReader(object):

    def __init__(self, filename):
        self.filename = filename
        self.fh = open(filename, "rb")
        self.reader = datafile.DataFileReader(self.fh, SysFlowReader(readers_schema=SysFlowSchema)) 
    
    def __iter__(self):
        return self

    def next(self):
        return next(self.reader)
    
    def __next__(self):
        return self.next()

   
    def close(self):
        self.reader.close()
