from enum import Enum

"""
.. module:: sysflow.objtypes
   :synopsis: This module represents each entity/flow/event class as a ID, and maps those ids to strings.
.. moduleauthor:: Teryl Taylor, Frederico Araujo
"""

class ObjectTypes(Enum):
    """
       **ObjectTypes**

       Enumeration representing each of the object types:
          HEADER = 0,
          CONT = 1, 
          PROC = 2,
          FILE = 3,
          PROC_EVT = 4,
          NET_FLOW = 5,
          FILE_FLOW = 6,
          FILE_EVT = 7
    """
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
