import avro.io
import io
import sysflow
from sysflow.objtypes import ObjectTypes
from sysflow import SpecificDatumReader as SysFlowReader 
from avro import datafile, io 
from sysflow.schema_classes import SCHEMA as SysFlowSchema
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

    def getProcess(self, oid):
        key = self.getProcessKey(oid)
        if key in self.processes:
            return self.processes[key]
        else:
            return None

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
                    return (ObjectTypes.HEADER, rec, None, None, None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ContainerClass):
                key = rec.id
                self.conts[key] = rec
                if self.retEntities:
                    return (ObjectTypes.CONT, self.header, rec, None,  None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ProcessClass):
                key = self.getProcessKey(rec.oid)
                self.processes[key] = rec
                if self.retEntities:
                    container = None
                    if rec.containerId is not None:
                        if not rec.containerId in self.conts:
                            print("ERROR: Cannot find container object for record.  This should not happen.") 
                        else:
                            container = self.conts[rec.containerId]
                    return (ObjectTypes.PROC, self.header, container, rec, None, None)
            elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.FileClass):
                key = rec.oid
                self.files[key] = rec
                if self.retEntities:
                    container = None
                    if rec.containerId is not None:
                        if not rec.containerId in self.conts:
                            print("ERROR: Cannot find container object for record.  This should not happen.") 
                        else:
                            container = self.conts[rec.containerId]
                    return (ObjectTypes.FILE, self.header, container, None, (rec, None), None)
            else:
                procOID = self.getProcessKey(rec.procOID)
                proc = None
                pproc = None
                container = None
                file1 = None
                file2 = None
                evt = None
                flow = None
                objType = ObjectTypes.NET_FLOW
                if not procOID in self.processes:
                    print("ERROR: Cannot find process object for record.  This should not happen.") 
                else:
                    proc = self.processes[procOID]
                    pproc = self.getProcess(proc.poid) if proc.poid is not None else None
                if proc is not None:
                    if proc.containerId is not None:
                        if not proc.containerId in self.conts:
                            print("ERROR: Cannot find container object for record.  This should not happen.") 
                        else:
                            container = self.conts[proc.containerId]
                if isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.event.FileEventClass):
                    fileOID = rec.fileOID
                    evt = rec
                    objType = ObjectTypes.FILE_EVT       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record.  This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                    fileOID2 = rec.newFileOID
                    if fileOID2 is not None:
                        if not fileOID2 in self.files:
                           print("ERROR: Cannot find file object for record.  This should not happen.") 
                        else:
                           file2 = self.files[fileOID2]

                elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.flow.FileFlowClass):
                    objType = ObjectTypes.FILE_FLOW      
                    fileOID = rec.fileOID
                    flow = rec       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record.  This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.event.ProcessEventClass):
                    objType = ObjectTypes.PROC_EVT
                    evt = rec
                else:
                    flow = rec
                files = (file1, file2) if file1 is not None or file2 is not None else None
                return (objType, self.header, container, pproc, proc, files, evt, flow)  
