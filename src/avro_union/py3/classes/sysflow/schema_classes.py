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
                    self.rec = SchemaClasses.sysflow.entity.ProcessClass()
            
            
            @property
            def rec(self):
                """
                :rtype: SchemaClasses.sysflow.entity.ProcessClass | SchemaClasses.sysflow.flow.ProcessFlowClass
                """
                return self._inner_dict.get('rec')
            
            @rec.setter
            def rec(self, value):
                #"""
                #:param SchemaClasses.sysflow.entity.ProcessClass | SchemaClasses.sysflow.flow.ProcessFlowClass value:
                #"""
                self._inner_dict['rec'] = value
            
            
        pass
        class entity(object):
            
            class ProcessClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.entity.Process")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.entity.ProcessClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.oid = str()
                        self.type = SchemaClasses.sysflow.type.ActionTypeClass.CREATED
                        self.hpid = int()
                        self.poid = str()
                        self.exe = str()
                        self.exeArgs = str()
                        self.uid = int()
                        self.userName = str()
                        self.gid = int()
                        self.groupName = str()
                        self.hostId = int()
                        self.duration = int()
                        self.threadCount = int()
                        self.childCount = int()
                        self.cid = str()
                        self.cname = str()
                        self.cimage = str()
                        self.cimagid = str()
                
                
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
                def type(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.ActionTypeClass
                    """
                    return self._inner_dict.get('type')
                
                @type.setter
                def type(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.ActionTypeClass value:
                    #"""
                    self._inner_dict['type'] = value
                
                
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
                
                
                @property
                def poid(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('poid')
                
                @poid.setter
                def poid(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['poid'] = value
                
                
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
                def hostId(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('hostId')
                
                @hostId.setter
                def hostId(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['hostId'] = value
                
                
                @property
                def duration(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('duration')
                
                @duration.setter
                def duration(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['duration'] = value
                
                
                @property
                def threadCount(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('threadCount')
                
                @threadCount.setter
                def threadCount(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['threadCount'] = value
                
                
                @property
                def childCount(self):
                    """
                    :rtype: int
                    """
                    return self._inner_dict.get('childCount')
                
                @childCount.setter
                def childCount(self, value):
                    #"""
                    #:param int value:
                    #"""
                    self._inner_dict['childCount'] = value
                
                
                @property
                def cid(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('cid')
                
                @cid.setter
                def cid(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['cid'] = value
                
                
                @property
                def cname(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('cname')
                
                @cname.setter
                def cname(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['cname'] = value
                
                
                @property
                def cimage(self):
                    """
                    :rtype: str
                    """
                    return self._inner_dict.get('cimage')
                
                @cimage.setter
                def cimage(self, value):
                    #"""
                    #:param str value:
                    #"""
                    self._inner_dict['cimage'] = value
                
                
                @property
                def cimagid(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('cimagid')
                
                @cimagid.setter
                def cimagid(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['cimagid'] = value
                
                
            pass
        class flow(object):
            
            class ProcessFlowClass(DictWrapper):
                
                """
                
                """
                
                
                RECORD_SCHEMA = get_schema_type("sysflow.flow.ProcessFlow")
                
                
                def __init__(self, inner_dict=None):
                    super(SchemaClasses.sysflow.flow.ProcessFlowClass, self).__init__(inner_dict)
                    if inner_dict is None:
                        self.type = SchemaClasses.sysflow.type.EventTypeClass.CLONE
                        self.procOID = str()
                        self.args = list()
                        self.ret = int()
                
                
                @property
                def type(self):
                    """
                    :rtype: SchemaClasses.sysflow.type.EventTypeClass
                    """
                    return self._inner_dict.get('type')
                
                @type.setter
                def type(self, value):
                    #"""
                    #:param SchemaClasses.sysflow.type.EventTypeClass value:
                    #"""
                    self._inner_dict['type'] = value
                
                
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
                def procOID(self):
                    """
                    :rtype: bytes
                    """
                    return self._inner_dict.get('procOID')
                
                @procOID.setter
                def procOID(self, value):
                    #"""
                    #:param bytes value:
                    #"""
                    self._inner_dict['procOID'] = value
                
                
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
        class type(object):
            
            class ActionTypeClass(object):
                
                """
                
                """
                
                CREATED = "CREATED"
                MODIFIED = "MODIFIED"
                REUP = "REUP"
                
            class EventTypeClass(object):
                
                """
                
                """
                
                CLONE = "CLONE"
                EXEC = "EXEC"
                EXIT = "EXIT"
                
            pass
            
__SCHEMA_TYPES = {
'sysflow.SysFlow': SchemaClasses.sysflow.SysFlowClass,
    'sysflow.entity.Process': SchemaClasses.sysflow.entity.ProcessClass,
    'sysflow.flow.ProcessFlow': SchemaClasses.sysflow.flow.ProcessFlowClass,
    'sysflow.type.ActionType': SchemaClasses.sysflow.type.ActionTypeClass,
    'sysflow.type.EventType': SchemaClasses.sysflow.type.EventTypeClass,
    
}
_json_converter = avrojson.AvroJsonConverter(use_logical_types=False, schema_types=__SCHEMA_TYPES)

