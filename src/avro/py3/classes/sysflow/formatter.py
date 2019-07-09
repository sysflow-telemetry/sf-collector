import sys, os, json, csv
import sysflow.utils as utils
from sysflow.objtypes import ObjectTypes, OBJECT_MAP
from collections import OrderedDict
from tabulate import tabulate

_default_fields = ['flow_type', 'proc_exe', 'proc_args', 'pproc_pid', 'proc_pid', 'proc_tid','op_flags', 'ts', 'end_ts', 'fd', 'ret_code', 'res', 'rcv_r_bytes', 'snd_w_bytes', 'cont_id']

_header_map = { 'flow_type': 'T',
                'op_flags': 'Op Flags',
                'ret_code': 'Ret',
                'ts': 'Start Time', 
                'end_ts': 'End Time',
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

class SFFormatter(object):

    def __init__(self, reader):  
        self.reader = reader
        
    def toJsonStdOut(self, fields=None):
        for objtype, header, cont, pproc, proc, files, evt, flow in self.reader:
            record = self._flatten(objtype, header, cont, pproc, proc, files, evt, flow, fields) 
            print(json.dumps(record))

    def toJsonFile(self, path, fields=None):
        with open(path, mode='w') as jsonfile:
            for objtype, header, cont, pproc, proc, files, evt, flow in self.reader:
                record = self._flatten(objtype, header, cont, pproc, proc, files, evt, flow, fields) 
                json.dump(record, jsonfile)

    def toCsvFile(self, path, fields=None, header=True): 
        first = True
        with open(path, mode='w') as csv_file:
            writer = None
            for objtype, header, cont, pproc, proc, files, evt, flow in self.reader:
                record = self._flatten(objtype, header, cont, pproc, proc, files, evt, flow, fields) 
                if first:
                  fieldnames = list(record.keys()) 
                  writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
                  first = False
                  if header:
                      writer.writeheader()
                writer.writerow(record)
            writer.close()
                #for r in self._flat_list:
                #    writer.writerow(r)

    def toStdOut(self, fields=_default_fields, pretty_headers=True, showindex=True):
        f = _default_fields if fields is None else fields
        headers = _header_map if pretty_headers else 'keys'
        bulkRecs = []
        first = True
        i = 0
        for objtype, header, cont, pproc, proc, files, evt, flow in self.reader:
            record = self._flatten(objtype, header, cont, pproc, proc, files, evt, flow, fields) 
            bulkRecs.append(record)
            i+=1
            if i == 100:
                if first:
                    print(tabulate(bulkRecs, headers=headers, showindex=showindex, tablefmt='github'))
                    first = False
                else:
                    print(tabulate(bulkRecs, showindex=showindex, tablefmt='github'))
                i = 0
                bulkRecs = []

        if len(bulkRecs) > 0:
           if first: 
               print(tabulate(bulkRecs, headers=headers, showindex=showindex, tablefmt='github'))
           else:
               print(tabulate(bulkRecs, showindex=showindex, tablefmt='github'))

    def _filter(self, fields):
        #This line replaces the convoluted nested loop below in Python 3.6> where dict comprehensions are ordered by default.
        #data = [ { k: d[k] for k in fields } for d in self._flat_list ] if fields is not None else self._flat_list
        data = [] if fields is not None else self._flat_list
        if fields is not None: 
            for d in self._flat_list:
                od = OrderedDict()
                for k in fields:
                    od[k]=d[k]
                data.append(od)
        return data

    def _flatten(self, objtype, header, cont, pproc, proc, files, evt, flow, fields):
        _flat_map = OrderedDict()
        evflow = evt or flow
        _flat_map['flow_type'] = OBJECT_MAP.get(objtype,'?')
        _flat_map['op_flags'] = utils.getOpFlagsStr(evflow.opFlags) if evflow is not None else ''
        _flat_map['ret_code'] = evflow.ret if evt is not None else '' 
        _flat_map['ts'] = utils.getTimeStr(evflow.ts) if evflow is not None else ''
        _flat_map['end_ts'] = utils.getTimeStr(evflow.endTs) if flow is not None else ''
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
        _flat_map['fd'] = flow.fd if objtype == ObjectTypes.FILE_FLOW or objtype == ObjectTypes.NET_FLOW else ''
        _flat_map['open_flags'] = flow.openFlags if objtype == ObjectTypes.FILE_FLOW else ''
        if objtype == ObjectTypes.FILE_FLOW or objtype == ObjectTypes.FILE_EVT:
            _flat_map['res'] = files[0].path if files is not None and files[0] is not None else ''
            _flat_map['res'] += ', ' + files[1].path if files is not None and files[1] is not None else ''
        elif objtype == ObjectTypes.NET_FLOW:
            _flat_map['res'] = utils.getNetFlowStr(flow)
            _flat_map['proto'] = evflow.proto
            _flat_map['sport'] = evflow.sport
            _flat_map['dport'] = evflow.dport
            _flat_map['sip'] = evflow.sip
            _flat_map['dip'] = evflow.dip
        else:
            _flat_map['res'] = ''
            _flat_map['rcv_r_bytes'] = evflow.numRRecvBytes if flow is not None else ''
            _flat_map['rcv_r_ops'] = evflow.numRRecvOps if flow is not None else ''
            _flat_map['snd_w_bytes'] = evflow.numWSendBytes if flow is not None else ''
            _flat_map['snd_w_ops'] = evflow.numWSendOps if flow is not None else ''
            _flat_map['cont_id'] = cont.id if cont is not None else ''
            _flat_map['cont_name'] = cont.name if cont is not None else ''
            _flat_map['cont_image_id'] = cont.imageid if cont is not None else ''
            _flat_map['cont_image'] = cont.image if cont is not None else ''
            _flat_map['cont_type'] = cont.type if cont is not None else ''
            _flat_map['cont_privileged'] = cont.privileged if cont is not None else ''
        if fields is not None: 
            od = OrderedDict()
            for k in fields:
                od[k]=_flat_map[k]
            return od
        return _flat_map
