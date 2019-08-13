import sys, os, json, csv
import sysflow.utils as utils
from sysflow.objtypes import ObjectTypes, OBJECT_MAP
from collections import OrderedDict
import tabulate
tabulate.PRESERVE_WHITESPACE = True
from tabulate import tabulate

"""
.. module:: sysflow.formatter
   :synopsis: This module allows SysFlow to be exported in formats other than avro including JSON, CSV, and tabular pretty print
.. moduleauthor:: Frederico Araujo, Teryl Taylor
"""

_default_fields = ['flow_type', 'proc_exe', 'proc_args', 'pproc_pid', 'proc_pid', 'proc_tid','op_flags', 'ts', 'end_ts', 'fd', 'ret_code', 'res', 'rcv_r_bytes', 'snd_w_bytes', 'cont_id']

_header_map = { 'idx': 'Evt #',
                'flow_type': 'T',
                'op_flags': 'Op Flags',
                'op_flags_bitmap': 'Op Flags',
                'ret_code': 'Ret',
                'ts': 'Start Time', 
                'ts_uts': 'Start Time', 
                'end_ts': 'End Time',
                'end_ts_uts': 'End Time',
                'proc_pid': 'PID',
                'proc_tid': 'TID',
                'proc_uid': 'UID',
                'proc_user': 'User', 
                'proc_gid': 'GID',
                'proc_group': 'Group',
                'proc_exe': 'Cmd', 
                'proc_args': 'Args',
                'proc_tty': 'TTY',
                'proc_create_ts': 'Proc. Creation Time',
                'pproc_pid': 'PPID',
                'pproc_gid': 'PGID',
                'pproc_uid': 'PUID',
                'pproc_group': 'PGroup',
                'pproc_tty': 'PTTY', 
                'pproc_user': 'PUser',
                'pproc_exe': 'PCmd',
                'pproc_args': 'PArgs',
                'pproc_create_ts': 'PProc. Creation Time',
                'fd': 'FD',
                'open_flags': 'Open Flags',
                'res': 'Resource',
                'proto': 'Protocol',
                'sport': 'SPort',
                'dport': 'DPort',
                'sip': 'SIP',
                'dip': 'DIP', 
                'rcv_r_bytes': 'NoBRead',
                'rcv_r_ops': 'NoOpsRead',
                'snd_w_bytes': 'NoBWrite',
                'snd_w_ops': 'NoOpsWrite',
                'cont_id': 'Cont ID',
                'cont_image_id': 'Image ID', 
                'cont_image': 'Image Name',
                'cont_name': 'Cont Name',
                'cont_type': 'Cont Type',
                'cont_privileged': 'Privileged'
              }

_colwidths = {  'idx': 6,
                'flow_type': 5,
                'op_flags': 12,
                'op_flags_bitmap': 5,
                'ret_code': 4,
                'ts': 26, 
                'ts_uts': 12, 
                'end_ts': 26,
                'end_ts_uts': 12,
                'proc_pid': 5,
                'proc_tid': 5,
                'proc_uid': 5,
                'proc_user': 8, 
                'proc_gid': 5,
                'proc_group': 8,
                'proc_exe': 30, 
                'proc_args': 45,
                'proc_tty': 5,
                'proc_create_ts': 12,
                'pproc_pid': 5,
                'pproc_gid': 5,
                'pproc_uid': 5,
                'pproc_group': 8,
                'pproc_tty': 5, 
                'pproc_user': 8,
                'pproc_exe': 30,
                'pproc_args': 45,
                'pproc_create_ts': 8,
                'fd': 5,
                'open_flags': 5,
                'res': 45,
                'proto': 5,
                'sport': 5,
                'dport': 5,
                'sip': 16,
                'dip': 16, 
                'rcv_r_bytes': 8,
                'rcv_r_ops': 8,
                'snd_w_bytes': 8,
                'snd_w_ops': 8,
                'cont_id': 16,
                'cont_image_id': 16, 
                'cont_image': 16,
                'cont_name': 16,
                'cont_type': 8,
                'cont_privileged': 5
              }


class SFFormatter(object):
    """
       **SFFormatter**

       This class takes a `FlattenedSFReader`, and exports SysFlow as either JSON, CSV or Pretty Print .
       Example Usage::

           reader = FlattenedSFReader(trace, False)
           formatter = SFFormatter(reader)
           fields=args.fields.split(',') if args.fields is not None else None
           if args.output == 'json':
               if args.file is not None:
                   formatter.toJsonFile(args.file, fields=fields)
               else:
                   formatter.toJsonStdOut(fields=fields)
           elif args.output == 'csv' and args.file is not None:
               formatter.toCsvFile(args.file, fields=fields)
           elif args.output == 'str':
               formatter.toStdOut(fields=fields)
       
       :param reader: A reader representing the sysflow file being read.
       :type reader: sysflow.reader.FlattenedSFReader
    """
    def __init__(self, reader):  
        self.reader = reader
   
    def applyFuncJson(self, func, fields=None):
        """Enables a delegate function to be applied to each JSON record read.

        :param func: delegate function of the form func(str) 
        :type func: function
        
        :param fields: a list of the SysFlow fields to be exported in the JSON.  See 
                       formatter.py for a list of fields
        :type fields: list
        """
        for r in self.reader:
            record = self._flatten(*r, fields) 
            func(json.dumps(record))
     
    def toJsonStdOut(self, fields=None):
        """Writes SysFlow as JSON to stdout.

        :param fields: a list of the SysFlow fields to be exported in the JSON.  See 
                       formatter.py for a list of fields
        :type fields: list
        """
        for r in self.reader:
            record = self._flatten(*r, fields) 
            print(json.dumps(record))

    def toJsonFile(self, path, fields=None):
        """Writes SysFlow to JSON file.

        :param path: the full path of the output file. 
        :type path: str
        
        :param fields: a list of the SysFlow fields to be exported in the JSON.  See 
                       formatter.py for a list of fields
        :type fields: list
        """
        with open(path, mode='w') as jsonfile:
            json.dump([self._flatten(*r, fields) for r in self.reader], jsonfile)

    def toCsvFile(self, path, fields=None, header=True): 
        """Writes SysFlow to CSV file.

        :param path: the full path of the output file. 
        :type path: str
        
        :param fields: a list of the SysFlow fields to be exported in the JSON.  See 
                       formatter.py for a list of fields
        :type fields: list
        """
        with open(path, mode='w') as csv_file:
            for idx, r in enumerate(self.reader):
                record = self._flatten(*r, fields) 
                if idx == 0:
                  fieldnames = list(record.keys()) 
                  writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
                  if header:
                      writer.writeheader()
                writer.writerow(record)

    def toStdOut(self, fields=_default_fields, pretty_headers=True, showindex=True):
        """Writes SysFlow as a tabular pretty print form to stdout.

        :param fields: a list of the SysFlow fields to be exported in the JSON.  See 
                       formatter.py for a list of fields
        :type fields: list
        
        :param pretty_headers: print table headers in pretty format. 
        :type pretty_headers: bool
        
        :param showindex: show record number. 
        :type showindex: bool
        """
        fields = _default_fields if fields is None else fields
        headers = _header_map if pretty_headers else 'keys'
        bulkRecs = []
        first = True

        for idx, r in enumerate(self.reader):
            record = self._flatten(*r, fields) 
            if showindex:
                record['idx'] = idx
                record.move_to_end('idx', last=False)
            for key, value in record.items():
                w = _colwidths[key] + 2
                data = '{0: <{width}}'.format(value, width=w)
                record[key] = data[:w] + (data[w:] and '..')
            bulkRecs.append(record)
            if idx > 0 and idx % 1000 == 0:
                if first:
                    print(tabulate(bulkRecs, headers=headers, tablefmt='github'))
                    first = False
                else:
                    print(tabulate(bulkRecs, tablefmt='github'))
                bulkRecs = []

        if len(bulkRecs) > 0:
           if first: 
               print(tabulate(bulkRecs, headers=headers, tablefmt='github'))
           else:
               print(tabulate(bulkRecs, tablefmt='github'))

    def _flatten(self, objtype, header, cont, pproc, proc, files, evt, flow, fields):
        _flat_map = OrderedDict()
        evflow = evt or flow
        _flat_map['flow_type'] = OBJECT_MAP.get(objtype,'?')
        _flat_map['op_flags'] = utils.getOpFlagsStr(evflow.opFlags) if evflow is not None else ''
        _flat_map['ret_code'] = evflow.ret if evt is not None else '' 
        _flat_map['ts'] = utils.getTimeStr(evflow.ts) if evflow is not None else ''
        _flat_map['ts_uts'] = evflow.ts if evflow is not None else ''
        _flat_map['end_ts'] = utils.getTimeStr(evflow.endTs) if flow is not None else ''
        _flat_map['end_ts_uts'] = evflow.endTs if flow is not None else ''
        _flat_map['proc_pid'] = proc.oid.hpid if proc is not None else ''
        _flat_map['proc_tid'] = evflow.tid if evflow is not None else ''
        _flat_map['proc_uid'] = proc.uid if proc is not None else ''
        _flat_map['proc_user'] = proc.userName if proc is not None else '' 
        _flat_map['proc_gid'] = proc.gid if proc is not None else ''
        _flat_map['proc_group'] = proc.groupName if proc is not None else ''
        _flat_map['proc_exe'] = proc.exe if proc is not None else '' 
        _flat_map['proc_args'] = proc.exeArgs if proc is not None else ''
        _flat_map['proc_tty'] = proc.tty if proc is not None else ''
        _flat_map['proc_create_ts'] = proc.oid.createTS if proc is not None else ''
        _flat_map['pproc_pid'] = pproc.oid.hpid if pproc is not None else ''
        _flat_map['pproc_gid'] = pproc.gid if pproc is not None else ''
        _flat_map['pproc_uid'] = pproc.uid if pproc is not None else ''
        _flat_map['pproc_group'] = pproc.groupName if pproc is not None else ''
        _flat_map['pproc_tty'] = pproc.tty if pproc is not None else ''
        _flat_map['pproc_user'] = pproc.userName if pproc is not None else ''
        _flat_map['pproc_exe'] = pproc.exe if pproc is not None else ''
        _flat_map['pproc_args'] = pproc.exeArgs if pproc is not None else ''
        _flat_map['pproc_create_ts'] = pproc.oid.createTS if pproc is not None else ''
        _flat_map['fd'] = flow.fd if flow is not None else ''
        _flat_map['open_flags'] = flow.openFlags if objtype == ObjectTypes.FILE_FLOW else ''
        
        if objtype in [ObjectTypes.FILE_FLOW, ObjectTypes.FILE_EVT]:
            _flat_map['res'] = files[0].path if files is not None and files[0] is not None else ''
            _flat_map['res'] += ', ' + files[1].path if files is not None and files[1] is not None else ''
        elif objtype in [ObjectTypes.NET_FLOW]:
            _flat_map['res'] = utils.getNetFlowStr(flow)
        else:
            _flat_map['res'] = ''

        _flat_map['proto'] = evflow.proto if objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['sport'] = evflow.sport if objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['dport'] = evflow.dport if objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['sip'] = evflow.sip if objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['dip'] = evflow.dip if objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['rcv_r_bytes'] = flow.numRRecvBytes if flow is not None else ''
        _flat_map['rcv_r_ops'] = flow.numRRecvOps if flow is not None else ''
        _flat_map['snd_w_bytes'] = flow.numWSendBytes if flow is not None else ''
        _flat_map['snd_w_ops'] = flow.numWSendOps if flow is not None else ''
        _flat_map['cont_id'] = cont.id if cont is not None else ''
        _flat_map['cont_name'] = cont.name if cont is not None else ''
        _flat_map['cont_image_id'] = cont.imageid if cont is not None else ''
        _flat_map['cont_image'] = cont.image if cont is not None else ''
        _flat_map['cont_type'] = cont.type if cont is not None else ''
        _flat_map['cont_privileged'] = cont.privileged if cont is not None else ''
        
        if fields: 
            od = OrderedDict()
            for k in fields:
                od[k]=_flat_map[k]
            return od
        
        return _flat_map
