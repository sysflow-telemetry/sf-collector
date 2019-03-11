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
        class type(object):
            
            class ActionTypeClass(object):
                
                """
                
                """
                
                CREATED = "CREATED"
                MODIFIED = "MODIFIED"
                REUP = "REUP"
                
            pass
            
__SCHEMA_TYPES = {
'sysflow.type.ActionType': SchemaClasses.sysflow.type.ActionTypeClass,
    
}
_json_converter = avrojson.AvroJsonConverter(use_logical_types=False, schema_types=__SCHEMA_TYPES)

