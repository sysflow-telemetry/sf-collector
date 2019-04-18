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
            
            class EventTypeClass(object):
                
                """
                
                """
                
                CLONE = "CLONE"
                EXEC = "EXEC"
                EXIT = "EXIT"
                
            pass
            
__SCHEMA_TYPES = {
'sysflow.flow.ProcessFlow': SchemaClasses.sysflow.flow.ProcessFlowClass,
    'sysflow.type.EventType': SchemaClasses.sysflow.type.EventTypeClass,
    
}
_json_converter = avrojson.AvroJsonConverter(use_logical_types=False, schema_types=__SCHEMA_TYPES)

