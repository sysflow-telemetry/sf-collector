from enum import Enum

class ObjectTypes(Enum):
    HEADER = 0
    CONT = 1 
    PROC = 2
    FILE = 3
    PROC_EVT = 4
    NET_FLOW = 5
    FILE_FLOW = 6
    FILE_EVT = 7

OBJECT_MAP = { 
    ObjectTypes.HEADER: "H", 
    ObjectTypes.CONT: "C", 
    ObjectTypes.PROC : "P",
    ObjectTypes.FILE : "F",
    ObjectTypes.PROC_EVT : "PE",
    ObjectTypes.NET_FLOW : "NF",
    ObjectTypes.FILE_FLOW : "FF",
    ObjectTypes.FILE_EVT : "FE"
    }
