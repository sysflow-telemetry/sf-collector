#!/usr/bin/env python3
from sysflow.reader import SFReader
import sys
import json
import sysflow
import itertools
import base64

primitives = (int, str, bool, float, bytes)

reader1 = SFReader(sys.argv[1])
reader2 = SFReader(sys.argv[2])

def compareAttributes(att1, att2, exit, msgs):
    if type(att1) != type(att2):
         msgs.append('Attribute types: ' + str(type(att1)) + ' and ' + str(type(att2)) + ' don\'t match.')
         exit[0] = True
         return False;
    if hasattr(att1,'__dict__'):
        f1attrs = [a for a in dir(att1) if not (a.startswith('__') or a == '_inner_dict' or a == 'RECORD_SCHEMA' or a == 'fromkeys' or a == 'get' or a == 'items' or a == 'keys' or a == 'pop' or a == 'popitem' or a == 'setdefault' or a == 'update' or a == 'values' or a == 'copy' or a == 'clear') ]
        f2attrs = [a for a in dir(att2) if not (a.startswith('__') or a == '_inner_dict' or a == 'RECORD_SCHEMA' or a == 'fromkeys' or a == 'get' or a == 'items' or a == 'keys' or a == 'pop' or a == 'popitem' or a == 'setdefault' or a == 'update' or a == 'values' or a == 'copy' or a == 'clear') ]
        if len(f1attrs) != len(f2attrs):
            msgs.append(str(type(att1)) + ' does not have the same number of attributes in each file')
            return False
        #print('Instance is a class type' + str(type(att1)))
        for attr in f1attrs:
            a1 = getattr(att1, attr)
            a2 = getattr(att2, attr)
            if(not compareAttributes(a1, a2, exit, msgs)):
                msgs.append('Attribute: ' + attr + ' does not have matching values')
                return False
    #if isinstance(att1, sysflow.schema_classes.SchemaClasses.sysflow.type.OIDClass):
       # print('Instance of OID class')
    elif isinstance(att1, list):
        if len(att1) != len(att2):
            msgs.append('lists are not the same length ' + str(len(att1)) + " " + str(len(att2)))
            return False
        i = 0
        for a in att1:
            if(not compareAttributes(a, att2[i], exit, msgs)):
                return False;
    elif isinstance(att1, primitives):
        if att1 != att2:
            if isinstance(att1, bytes):
                 msgs.append('Attribute values do not match ' + str(base64.b64encode(att1)) +  ' ' + str(base64.b64encode(att2)))
            else:
                 msgs.append('Attribute values do not match ' + str(att1) +  ' ' + str(att2))
            return False
    elif att1 is None:
        if not att2 is None:
             return False
    else:
        print('Uh oh, class not supported!' + str(type(att1)))
        return False
    return True   
 



exit = [False]
msgs = []
recNum = 0
failures = 0

print('Beginning test cases between files: {} and {}'.format(sys.argv[1], sys.argv[2]))

for flow1, flow2 in itertools.zip_longest(reader1, reader2, fillvalue=None):
    #flow2 = reader2.next();
    if not (flow1 is None or flow2 is None):
         msgs.clear()
         if not compareAttributes(flow1.rec, flow2.rec, exit, msgs):
             print('[ FAILED ] Record: {} does not match'.format(recNum))
             failures += 1
             for m in msgs:
                 print(m)
         if exit[0] == True:
             print('[ FAILED ] Stopping Test. Test case failed with {} failures before stopping'.format(failures))
             sys.exit(1)
         recNum += 1
    else:
        print('[ FAILED ]  Files do not have the same number of records')
        failures += 1

#if not reader2.is_EOF():
#    print('[ FAILED ]  Files do not have the same number of records.')
#    failures += 1

if failures > 0:
    print('[ FAILED ] Test case failed with {} failures before stopping'.format(failures));
    sys.exit(1)
else:
    print('[ PASSED ] Test case complete')
