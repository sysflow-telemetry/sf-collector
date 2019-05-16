import json
import os.path
import decimal
import datetime
import six
from avrogen.dict_wrapper import DictWrapper
from avrogen import avrojson
from avro import schema as avro_schema
if six.PY3:    from avro.schema import SchemaFromJSONData as make_avsc_object
    
else:
    from avro.schema import make_avsc_object
    


def __read_file(file_name):
    with open(file_name, "r") as f:
        return f.read()

def __get_names_and_schema(file_name):
    names = avro_schema.Names()
    schema = make_avsc_object(json.loads(__read_file(file_name)), names)
    return names, schema

__NAMES, SCHEMA = __get_names_and_schema(os.path.join(os.path.dirname(__file__), "schema.avsc"))
__SCHEMAS = {}
def get_schema_type(fullname):
    return __SCHEMAS.get(fullname)
__SCHEMAS = dict((n.fullname.lstrip("."), n) for n in six.itervalues(__NAMES.names))


class SchemaClasses(object):
    
    
    pass
    class sysflow(object):
        
        class SysFlowClass(DictWrapper):
            
            """
            
            """
            
            
            RECORD_SCHEMA = get_schema_type("sysflow.SysFlow")
            
            
            def __init__(self, inner_dict=None):
                super(SchemaClasses.sysflow.SysFlowClass, self).__init__(inner_dict)
                if inner_dict is None:
                    self.rec = SchemaClasses.sysflow.entity.SFHeaderClass()
            
            
            @property
            def rec(self):
                """
                :rtype: SchemaClasses.sysflow.entity.SFHeaderClass | SchemaClasses.sysflow.entity.ContainerClass | SchemaClasses.sysflow.entity.ProcessClass | SchemaClasses.sysflow.entity.FileClass | SchemaClasses.sysflow.event.ProcessEventClass | SchemaClasses.sysflow.flow.NetworkFlowClass | SchemaClasses.sysflow.flow.FileFlowClass | SchemaClasses.sysflow.event.FileEventClass | SchemaClasses.sysflow.event.NetworkEventClass
                """
                return self._inner_dict.get('rec')
            
            @rec.setter
            def rec(self, value):
                #"""
                #:param SchemaClasses.sysflow.entity.SFHeaderClass | SchemaClasses.sysflow.entity.ContainerClass | SchemaClasses.sysflow.entity.ProcessClass | SchemaClasses.sysflow.entity.FileClass | SchemaClasses.sysflow.event.ProcessEventClass | SchemaClasses.sysflow.flow.NetworkFlowClass | SchemaClasses.sysflow.flow.FileFlowClass | SchemaClasses.sysflow.event.FileEventClass | SchemaClasses.sysflow.event.NetworkEventClass value:
                #"""
                self._inner_dict['rec'] = value
            
            
        pass
        class entity(object):
            
            class ContainerClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.entity.Container")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.entity.ContainerClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.id = str()
                        self.name = str()
                        self.image = str()
                        self.imageid = str()
                        self.type = SchemaClasses.sysflow.type.ContainerTypeClass.CT_DOCKER
                        self.privileged = bool()
                
                
                @property
                def id(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('id')
                
                @id.setter
                def id(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['id'] = value
                
                
                @property
                def name(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('name')
                
                @name.setter
                def name(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['name'] = value
                
                
                @property
                def image(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('image')
                
                @image.setter
                def image(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['image'] = value
                
                
                @property
                def imageid(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('imageid')
                
                @imageid.setter
                def imageid(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['imageid'] = value
                
                
                @property
                def type(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.ContainerTypeClass
                    """
                    return self._inner_dict.get('type')
                
                @type.setter
                def type(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.ContainerTypeClass value:
                    #"""
                    self._inner_dict['type'] = value
                
                
                @property
                def privileged(self):
                    """
                    :rtype: bool
                    """
                    return self._inner_dict.get('privileged')
                
                @privileged.setter
                def privileged(self, value):
                    #"""
                    #:param bool value:
                    #"""
                    self._inner_dict['privileged'] = value
                
                
            class FileClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.entity.File")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.entity.FileClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.state = SchemaClasses.sysflow.type.SFObjectStateClass.CREATED
                        self.oid = str()
                        self.restype = int()
                        self.path = str()
                        self.containerId = None
                
                
                @property
                def state(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.SFObjectStateClass
                    """
                    return self._inner_dict.get('state')
                
                @state.setter
                def state(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.SFObjectStateClass value:
                    #"""
                    self._inner_dict['state'] = value
                
                
                @property
                def oid(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('oid')
                
                @oid.setter
                def oid(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['oid'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def restype(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('restype')
                
                @restype.setter
                def restype(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['restype'] = value
                
                
                @property
                def path(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('path')
                
                @path.setter
                def path(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['path'] = value
                
                
                @property
                def containerId(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('containerId')
                
                @containerId.setter
                def containerId(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['containerId'] = value
                
                
            class ProcessClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.entity.Process")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.entity.ProcessClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.state = SchemaClasses.sysflow.type.SFObjectStateClass.CREATED
                        self.oid = SchemaClasses.sysflow.type.OIDClass()
                        self.poid = None
                        self.exe = str()
                        self.exeArgs = str()
                        self.uid = int()
                        self.userName = str()
                        self.gid = int()
                        self.groupName = str()
                        self.tty = bool()
                        self.containerId = None
                
                
                @property
                def state(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.SFObjectStateClass
                    """
                    return self._inner_dict.get('state')
                
                @state.setter
                def state(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.SFObjectStateClass value:
                    #"""
                    self._inner_dict['state'] = value
                
                
                @property
                def oid(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('oid')
                
                @oid.setter
                def oid(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['oid'] = value
                
                
                @property
                def poid(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('poid')
                
                @poid.setter
                def poid(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['poid'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def exe(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('exe')
                
                @exe.setter
                def exe(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['exe'] = value
                
                
                @property
                def exeArgs(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('exeArgs')
                
                @exeArgs.setter
                def exeArgs(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['exeArgs'] = value
                
                
                @property
                def uid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('uid')
                
                @uid.setter
                def uid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['uid'] = value
                
                
                @property
                def userName(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('userName')
                
                @userName.setter
                def userName(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['userName'] = value
                
                
                @property
                def gid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('gid')
                
                @gid.setter
                def gid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['gid'] = value
                
                
                @property
                def groupName(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('groupName')
                
                @groupName.setter
                def groupName(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['groupName'] = value
                
                
                @property
                def tty(self):
                    """
                    :rtype: bool
                    """
                    return self._inner_dict.get('tty')
                
                @tty.setter
                def tty(self, value):
                    #"""
                    #:param bool value:
                    #"""
                    self._inner_dict['tty'] = value
                
                
                @property
                def containerId(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('containerId')
                
                @containerId.setter
                def containerId(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['containerId'] = value
                
                
            class SFHeaderClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.entity.SFHeader")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.entity.SFHeaderClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.version = int()
                        self.exporter = str()
                
                
                @property
                def version(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('version')
                
                @version.setter
                def version(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['version'] = value
                
                
                @property
                def exporter(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('exporter')
                
                @exporter.setter
                def exporter(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['exporter'] = value
                
                
            pass
        class event(object):
            
            class FileEventClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.event.FileEvent")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.event.FileEventClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.procOID = SchemaClasses.sysflow.type.OIDClass()
                        self.tid = int()
                        self.opFlags = int()
                        self.fileOID = str()
                        self.ret = int()
                        self.newFileOID = None
                
                
                @property
                def procOID(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def tid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('tid')
                
                @tid.setter
                def tid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['tid'] = value
                
                
                @property
                def opFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('opFlags')
                
                @opFlags.setter
                def opFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['opFlags'] = value
                
                
                @property
                def fileOID(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('fileOID')
                
                @fileOID.setter
                def fileOID(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['fileOID'] = value
                
                
                @property
                def ret(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ret')
                
                @ret.setter
                def ret(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ret'] = value
                
                
                @property
                def newFileOID(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('newFileOID')
                
                @newFileOID.setter
                def newFileOID(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['newFileOID'] = value
                
                
            class NetworkEventClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.event.NetworkEvent")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.event.NetworkEventClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.procOID = SchemaClasses.sysflow.type.OIDClass()
                        self.tid = int()
                        self.opFlags = int()
                        self.sip = int()
                        self.sport = int()
                        self.dip = int()
                        self.dport = int()
                        self.proto = int()
                        self.ret = int()
                
                
                @property
                def procOID(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def tid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('tid')
                
                @tid.setter
                def tid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['tid'] = value
                
                
                @property
                def opFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('opFlags')
                
                @opFlags.setter
                def opFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['opFlags'] = value
                
                
                @property
                def sip(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('sip')
                
                @sip.setter
                def sip(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['sip'] = value
                
                
                @property
                def sport(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('sport')
                
                @sport.setter
                def sport(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['sport'] = value
                
                
                @property
                def dip(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('dip')
                
                @dip.setter
                def dip(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['dip'] = value
                
                
                @property
                def dport(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('dport')
                
                @dport.setter
                def dport(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['dport'] = value
                
                
                @property
                def proto(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('proto')
                
                @proto.setter
                def proto(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['proto'] = value
                
                
                @property
                def ret(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ret')
                
                @ret.setter
                def ret(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ret'] = value
                
                
            class ProcessEventClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.event.ProcessEvent")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.event.ProcessEventClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.procOID = SchemaClasses.sysflow.type.OIDClass()
                        self.tid = int()
                        self.opFlags = int()
                        self.args = list()
                        self.ret = int()
                
                
                @property
                def procOID(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def tid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('tid')
                
                @tid.setter
                def tid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['tid'] = value
                
                
                @property
                def opFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('opFlags')
                
                @opFlags.setter
                def opFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['opFlags'] = value
                
                
                @property
                def args(self):
                    """
                    :rtype: list[str]
                    """
                    return self._inner_dict.get('args')
                
                @args.setter
                def args(self, value):
                    #"""
                    #:param list[str] value:
                    #"""
                    self._inner_dict['args'] = value
                
                
                @property
                def ret(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ret')
                
                @ret.setter
                def ret(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ret'] = value
                
                
            pass
        class flow(object):
            
            class FileFlowClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.flow.FileFlow")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.flow.FileFlowClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.procOID = SchemaClasses.sysflow.type.OIDClass()
                        self.tid = int()
                        self.opFlags = int()
                        self.openFlags = int()
                        self.fileOID = str()
                        self.fd = int()
                        self.numRRecvOps = int()
                        self.numWSendOps = int()
                        self.numRRecvBytes = int()
                        self.numWSendBytes = int()
                
                
                @property
                def procOID(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def tid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('tid')
                
                @tid.setter
                def tid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['tid'] = value
                
                
                @property
                def opFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('opFlags')
                
                @opFlags.setter
                def opFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['opFlags'] = value
                
                
                @property
                def openFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('openFlags')
                
                @openFlags.setter
                def openFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['openFlags'] = value
                
                
                @property
                def endTs(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('endTs')
                
                @endTs.setter
                def endTs(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['endTs'] = value
                
                
                @property
                def fileOID(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('fileOID')
                
                @fileOID.setter
                def fileOID(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['fileOID'] = value
                
                
                @property
                def fd(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('fd')
                
                @fd.setter
                def fd(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['fd'] = value
                
                
                @property
                def numRRecvOps(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numRRecvOps')
                
                @numRRecvOps.setter
                def numRRecvOps(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numRRecvOps'] = value
                
                
                @property
                def numWSendOps(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numWSendOps')
                
                @numWSendOps.setter
                def numWSendOps(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numWSendOps'] = value
                
                
                @property
                def numRRecvBytes(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numRRecvBytes')
                
                @numRRecvBytes.setter
                def numRRecvBytes(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numRRecvBytes'] = value
                
                
                @property
                def numWSendBytes(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numWSendBytes')
                
                @numWSendBytes.setter
                def numWSendBytes(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numWSendBytes'] = value
                
                
            class NetworkFlowClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.flow.NetworkFlow")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.flow.NetworkFlowClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.procOID = SchemaClasses.sysflow.type.OIDClass()
                        self.tid = int()
                        self.opFlags = int()
                        self.sip = int()
                        self.sport = int()
                        self.dip = int()
                        self.dport = int()
                        self.proto = int()
                        self.fd = int()
                        self.numRRecvOps = int()
                        self.numWSendOps = int()
                        self.numRRecvBytes = int()
                        self.numWSendBytes = int()
                
                
                @property
                def procOID(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.OIDClass
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.OIDClass value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
                @property
                def ts(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('ts')
                
                @ts.setter
                def ts(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['ts'] = value
                
                
                @property
                def tid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('tid')
                
                @tid.setter
                def tid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['tid'] = value
                
                
                @property
                def opFlags(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('opFlags')
                
                @opFlags.setter
                def opFlags(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['opFlags'] = value
                
                
                @property
                def endTs(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('endTs')
                
                @endTs.setter
                def endTs(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['endTs'] = value
                
                
                @property
                def sip(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('sip')
                
                @sip.setter
                def sip(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['sip'] = value
                
                
                @property
                def sport(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('sport')
                
                @sport.setter
                def sport(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['sport'] = value
                
                
                @property
                def dip(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('dip')
                
                @dip.setter
                def dip(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['dip'] = value
                
                
                @property
                def dport(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('dport')
                
                @dport.setter
                def dport(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['dport'] = value
                
                
                @property
                def proto(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('proto')
                
                @proto.setter
                def proto(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['proto'] = value
                
                
                @property
                def fd(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('fd')
                
                @fd.setter
                def fd(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['fd'] = value
                
                
                @property
                def numRRecvOps(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numRRecvOps')
                
                @numRRecvOps.setter
                def numRRecvOps(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numRRecvOps'] = value
                
                
                @property
                def numWSendOps(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numWSendOps')
                
                @numWSendOps.setter
                def numWSendOps(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numWSendOps'] = value
                
                
                @property
                def numRRecvBytes(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numRRecvBytes')
                
                @numRRecvBytes.setter
                def numRRecvBytes(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numRRecvBytes'] = value
                
                
                @property
                def numWSendBytes(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('numWSendBytes')
                
                @numWSendBytes.setter
                def numWSendBytes(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['numWSendBytes'] = value
                
                
            pass
        class type(object):
            
            class ContainerTypeClass(object):
                
                """
                
                """
                
                CT_DOCKER = "CT_DOCKER"
                CT_LXC = "CT_LXC"
                CT_LIBVIRT_LXC = "CT_LIBVIRT_LXC"
                CT_MESOS = "CT_MESOS"
                CT_RKT = "CT_RKT"
                CT_CUSTOM = "CT_CUSTOM"
                
            class OIDClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.type.OID")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.type.OIDClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.hpid = int()
                
                
                @property
                def createTS(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('createTS')
                
                @createTS.setter
                def createTS(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['createTS'] = value
                
                
                @property
                def hpid(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('hpid')
                
                @hpid.setter
                def hpid(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['hpid'] = value
                
                
            class SFObjectStateClass(object):
                
                """
                
                """
                
                CREATED = "CREATED"
                MODIFIED = "MODIFIED"
                REUP = "REUP"
                
            pass
            
__SCHEMA_TYPES = {
'sysflow.SysFlow': SchemaClasses.sysflow.SysFlowClass,
    'sysflow.entity.Container': SchemaClasses.sysflow.entity.ContainerClass,
    'sysflow.entity.File': SchemaClasses.sysflow.entity.FileClass,
    'sysflow.entity.Process': SchemaClasses.sysflow.entity.ProcessClass,
    'sysflow.entity.SFHeader': SchemaClasses.sysflow.entity.SFHeaderClass,
    'sysflow.event.FileEvent': SchemaClasses.sysflow.event.FileEventClass,
    'sysflow.event.NetworkEvent': SchemaClasses.sysflow.event.NetworkEventClass,
    'sysflow.event.ProcessEvent': SchemaClasses.sysflow.event.ProcessEventClass,
    'sysflow.flow.FileFlow': SchemaClasses.sysflow.flow.FileFlowClass,
    'sysflow.flow.NetworkFlow': SchemaClasses.sysflow.flow.NetworkFlowClass,
    'sysflow.type.ContainerType': SchemaClasses.sysflow.type.ContainerTypeClass,
    'sysflow.type.OID': SchemaClasses.sysflow.type.OIDClass,
    'sysflow.type.SFObjectState': SchemaClasses.sysflow.type.SFObjectStateClass,
    
}
_json_converter = avrojson.AvroJsonConverter(use_logical_types=False, schema_types=__SCHEMA_TYPES)

