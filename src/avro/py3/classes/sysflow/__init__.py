

from .schema_classes import SchemaClasses, SCHEMA as my_schema, get_schema_type
from avro.io import DatumReader


class SpecificDatumReader(DatumReader):
    SCHEMA_TYPES = {
        "sysflow.SysFlow": SchemaClasses.sysflow.SysFlowClass,
        "sysflow.entity.Container": SchemaClasses.sysflow.entity.ContainerClass,
        "sysflow.entity.File": SchemaClasses.sysflow.entity.FileClass,
        "sysflow.entity.Process": SchemaClasses.sysflow.entity.ProcessClass,
        "sysflow.entity.SFHeader": SchemaClasses.sysflow.entity.SFHeaderClass,
        "sysflow.event.FileEvent": SchemaClasses.sysflow.event.FileEventClass,
        "sysflow.event.NetworkEvent": SchemaClasses.sysflow.event.NetworkEventClass,
        "sysflow.event.ProcessEvent": SchemaClasses.sysflow.event.ProcessEventClass,
        "sysflow.flow.FileFlow": SchemaClasses.sysflow.flow.FileFlowClass,
        "sysflow.flow.NetworkFlow": SchemaClasses.sysflow.flow.NetworkFlowClass,
        "sysflow.type.ContainerType": SchemaClasses.sysflow.type.ContainerTypeClass,
        "sysflow.type.OID": SchemaClasses.sysflow.type.OIDClass,
        "sysflow.type.SFObjectState": SchemaClasses.sysflow.type.SFObjectStateClass,
    }
    def __init__(self, readers_schema=None, **kwargs):
        writers_schema = kwargs.pop("writers_schema", readers_schema)
        writers_schema = kwargs.pop("writer_schema", writers_schema)
        super(SpecificDatumReader, self).__init__(writers_schema, readers_schema, **kwargs)
    def read_record(self, writers_schema, readers_schema, decoder):
        
        result = super(SpecificDatumReader, self).read_record(writers_schema, readers_schema, decoder)
        
        if readers_schema.fullname in SpecificDatumReader.SCHEMA_TYPES:
            result = SpecificDatumReader.SCHEMA_TYPES[readers_schema.fullname](result)
        
        return result