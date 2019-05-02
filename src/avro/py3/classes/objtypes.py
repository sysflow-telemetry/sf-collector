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
