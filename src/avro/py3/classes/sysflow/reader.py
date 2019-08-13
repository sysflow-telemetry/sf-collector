import avro.io
import io
import sysflow
from sysflow.objtypes import ObjectTypes
from sysflow import SpecificDatumReader as SysFlowReader 
from avro import datafile, io 
from sysflow.schema_classes import SCHEMA as SysFlowSchema
from avro import datafile, io 
from uuid import UUID

"""
.. module:: sysflow.reader
   :synopsis: All readers for reading sysflow are defined here.
.. moduleauthor:: Teryl Taylor, Frederico Araujo
"""

class SFReader(object):
    """
       **SFReader**

       This class loads a raw sysflow file, and returns each entity/flow one by one.
       It is the user's responsibility to link the related objects together through the OID. 
       This class supports the python iterator design pattern.
       Example Usage::
       
              reader = SFReader("./sysflowfile.sf")
              for sf in reader:
                  rec = sf.rec 
                  if isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.SFHeaderClass):
                     //do something with the header object
                  elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ContainerClass):
                     //do something with the container object
                  elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.entity.ProcessClass):
                     //do something with the Process object
                  ....
       
       :param filename: the name of the sysflow file to be read.
       :type filename: str
    """
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
    """
       **FlattenedSFReader**

       This class loads a raw sysflow file, and links all Entities (header, process, container, files) with
       the current flow or event in the file.  As a result, the user does not have to manage this information.
       This class supports the python iterator design pattern.
       Example Usage::

              reader = FlattenedSFReader("./sysflowfile.sf", False)
              for objtype, header, cont, proc, files, evt, flow  in reader:
                  exe = proc.exe + ' ' + proc.exeArgs if proc is not None else ''
                  pid = proc.oid.hpid if proc is not None else ''
                  evflow = evt or flow
                  tid = evflow.tid if evflow is not None else ''
                  opFlags = utils.getOpFlagsStr(evflow.opFlags) if evflow is not None else '' 
                  sTime = utils.getTimeStr(evflow.ts) if evflow is not None else ''
                  eTime = utils.getTimeStr(evflow.endTs) if flow is not None else ''
                  ret = evflow.ret if evt is not None else ''
                  res1 = ''
                  if objtype == ObjectTypes.FILE_FLOW or objtype == ObjectTypes.FILE_EVT:
                      res1 = files[0].path
                  elif objtype == ObjectTypes.NET_FLOW:
                      res1 = utils.getNetFlowStr(flow) 
                  numBReads = evflow.numRRecvBytes if flow is not None else ''
                  numBWrites = evflow.numWSendBytes if flow is not None else ''
    
                  res2 = files[1].path if files is not None and files[1] is not None else ''
                  cont = cont.id if cont is not None else '' 
                  print("|{0:30}|{1:9}|{2:26}|{3:26}|{4:30}|{5:8}|{6:8}|".format(exe, opFlags, sTime, eTime, res1, numBReads, numBWrites))

       :param filename: the name of the sysflow file to be read.
       :type filename: str
       :param retEntities: If True, the reader will return entity objects by themselves as they are seen in the sysflow file. 
                           In this case, all other objects will be set to None
       :type retEntities: bool

       **Iterator**
        Reader returns a tuple of objects in the following order:

        **objtype** (:class:`sysflow.objtypes.ObjectTypes`) The type of entity or flow returned.
 
        **header** (:class:`sysflow.entity.SFHeader`) The header entity of the file.
        
        **cont** (:class:`sysflow.entity.Container`) The container associated with the flow/evt, or None if no container.
  
        **proc** (:class:`sysflow.entity.Process`) The process associated with the flow/evt.

        **files** (tuple of :class:`sysflow.entity.File`) Any files associated with the flow/evt.

        **evt** (:class:`sysflow.event.{ProcessEvent,FileEvent}`) If the record is an event, it will be returned here. Otherwise this variable will be None. objtype will indicate the type of event.

        **flow** (:class:`sysflow.flow.{NetworkFlow,FileFlow}`) If the record is a flow, it will be returned here. Otherwise this variable will be None. objtype will indicate the type of flow.
    """
    def __init__(self, filename, retEntities=False):
        super().__init__(filename)
        self.processes = dict()
        self.files = dict()
        self.conts = dict()
        self.header = None
        self.retEntities = retEntities

    def getProcess(self, oid):
        """Returns a Process Object given a process object id.

        :param oid: the object id of the Process Object requested
        :type oid: sysflow.type.OID

        :rtype: sysflow.entity.Process
        :return: the desired process object or None if no process object is available.
        """
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
                    print("ERROR: Cannot find process object for record. This should not happen.") 
                else:
                    proc = self.processes[procOID]
                    pproc = self.getProcess(proc.poid) if proc.poid is not None else None
                if proc is not None:
                    if proc.containerId is not None:
                        if not proc.containerId in self.conts:
                            print("ERROR: Cannot find container object for record. This should not happen.") 
                        else:
                            container = self.conts[proc.containerId]
                if isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.event.FileEventClass):
                    fileOID = rec.fileOID
                    evt = rec
                    objType = ObjectTypes.FILE_EVT       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record. This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                    fileOID2 = rec.newFileOID
                    if fileOID2 is not None:
                        if not fileOID2 in self.files:
                           print("ERROR: Cannot find file object for record. This should not happen.") 
                        else:
                           file2 = self.files[fileOID2]

                elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.flow.FileFlowClass):
                    objType = ObjectTypes.FILE_FLOW      
                    fileOID = rec.fileOID
                    flow = rec       
                    if not fileOID in self.files:
                        print("ERROR: Cannot find file object for record. This should not happen.") 
                    else:
                        file1 = self.files[fileOID]
                elif isinstance(rec, sysflow.schema_classes.SchemaClasses.sysflow.event.ProcessEventClass):
                    objType = ObjectTypes.PROC_EVT
                    evt = rec
                else:
                    flow = rec
                files = (file1, file2) if file1 is not None or file2 is not None else None
                return (objType, self.header, container, pproc, proc, files, evt, flow)  
