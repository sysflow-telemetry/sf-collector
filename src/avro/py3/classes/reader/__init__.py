import avro.io
import io
import sysflow
from sysflow import SpecificDatumReader as SysFlowReader 
from avro import datafile, io 
from sysflow.schema_classes import SCHEMA as SysFlowSchema
from sysflow.sysflow import SysFlow
from avro import datafile, io 
from uuid import UUID
class SFReader(object):

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



class FlattenedSFReader(SFReader):
    def __init__(self, filename, retEntities=False):
        super().__init__(filename)
        self.processes = dict()
        self.files = dict()
        self.conts = dict()
        self.header = None
        self.retEntities = retEntities

    def getProcessKey(self, oid):
         hpid = oid.hpid
         createTS = oid.createTS
         key = hpid.to_bytes((hpid.bit_length() + 7) // 8, byteorder='little')
         key += createTS.to_bytes((createTS.bit_length() + 7) // 8, byteorder='little')
         return key
         
    def __next__(self):
        while True:
            sf = super().next()
            rec = sf.rec
            if isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.SFHeaderClass):
                self.header = rec
                if self.retEntities:
                    return (rec, None, None, None, None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ProcessClass):
                key = self.getProcessKey(rec.oid)
                self.processes[key] = rec
                if self.retEntities:
                    return (self.header, rec, None, None, None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ContainerClass):
                key = rec.id
                self.conts[key] = rec
                if self.retEntities:
                    return (self.header, None, rec, None, None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.FileClass):
                key = rec.oid
                self.files[key] = rec
                if self.retEntities:
                    return (self.header, None, None, None, rec, None)
            else:
                procOID = self.getProcessKey(rec.procOID)
                proc = None
                container = None
                file1 = None
                file2 = None
                if not procOID in self.processes:
                    print("ERROR: Cannot find process object for record.  This should not happen.") 
                else:
                    proc = self.processes[procOID]
                if proc is not None:
                    if proc.containerId is not None:
                        if not proc.containerId in self.conts:
                            print("ERROR: Cannot find container object for record.  This should not happen.") 
                        else:
                            container = self.conts[proc.containerId]
                if isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.event.FileEventClass):
                    fileOID = rec.fileOID       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record.  This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                    fileOID2 = self.newFileOID
                    if fileOID2 is not None:
                        if not fileOID2 in self.files:
                           print("ERROR: Cannot find file object for record.  This should not happen.") 
                        else:
                           file2 = self.files[fileOID2]

                elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.flow.FileFlowClass):
                    fileOID = rec.fileOID       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record.  This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                return (self.header, proc, container, rec, file1, file2)  
