from reader import FlattenedSFReader
import sys

from datetime import datetime
reader = FlattenedSFReader(sys.argv[1], False)

OP_CLONE = (1 << 0) 
OP_EXEC = (1 << 1)
OP_EXIT = (1 << 2)
OP_SETUID = (1 << 3)
OP_SETNS = (1 << 4)
OP_ACCEPT = (1 << 5) 
OP_CONNECT = (1 << 6) 
OP_OPEN = (1 << 7)
OP_READ_RECV = (1 << 8)
OP_WRITE_SEND = (1 << 9) 
OP_CLOSE = (1 << 10) 
OP_TRUNCATE = (1 << 11)
OP_DIGEST = (1 << 12) 
OP_MKDIR = (1 << 13)
OP_RMDIR = (1 << 14)
OP_LINK  = (1 << 15)
OP_UNLINK = (1 << 16) 
OP_SYMLINK = (1 << 17)
OP_RENAME = (1 << 18)

NANO_TO_SECS = 1000000000

print("|{0:50}|{1:5}|{2:5}|{3:9}|{4:26}|{5:26}|{6:5}|{7:45}|{8:8}|{9:8}|{10:45}|{11:10}|".format("Process","PID","TID","Op Flags", "Start Time", "End Time", "Ret", "Resource 1", "NBRead", "NBWrite", "Resource 2", "Cont")) 

def isFileEvent(opFlags):
    return (opFlags == OP_MKDIR or opFlags == OP_RMDIR or opFlags == OP_LINK or opFlags == OP_SYMLINK or opFlags == OP_UNLINK or opFlags == OP_RENAME)

def isProcessEvent(opFlags):
    return (opFlags == OP_CLONE or opFlags == OP_EXEC or opFlags == OP_EXIT or opFlags == OP_SETUID)

def getOpFlags(opFlags):
    ops = ""
    ops +=  "MKDIR" if (opFlags & OP_MKDIR) else "";
    ops +=  "RMDIR" if (opFlags & OP_RMDIR) else "";
    ops +=  "LINK" if (opFlags & OP_LINK) else "";
    ops +=  "SYMLINK" if (opFlags & OP_SYMLINK) else  "";
    ops +=  "UNLINK" if (opFlags & OP_UNLINK) else  "";
    ops +=  "RENAME" if (opFlags & OP_RENAME) else  "";

    if(len(ops) > 0):
        return ops
    
    ops +=  "CLONE" if (opFlags & OP_CLONE) else "";
    ops +=  "EXEC" if (opFlags & OP_EXEC) else "";
    ops +=  "EXIT" if (opFlags & OP_EXIT) else "";
    ops +=  "SETUID" if (opFlags & OP_SETUID) else  "";
    
    if(len(ops) > 0):
        return ops

    ops +=  "O" if (opFlags & OP_OPEN) else  " ";
    ops +=  "A" if (opFlags & OP_ACCEPT) else " ";
    ops +=  "C" if (opFlags & OP_CONNECT) else  " ";
    ops +=  "W" if (opFlags & OP_WRITE_SEND)  else " ";
    ops +=  "R" if (opFlags & OP_READ_RECV)  else " ";
    ops +=  "N" if (opFlags & OP_SETNS)  else " ";
    ops +=  "C" if (opFlags & OP_CLOSE)  else " ";
    ops +=  "T" if (opFlags & OP_TRUNCATE) else " ";
    ops +=  "D" if (opFlags & OP_DIGEST)  else " ";
    return ops


for header, proc, cont, evflow, file1, file2  in reader:
    exe = proc.exe + ' ' + proc.exeArgs if proc is not None else ''
    pid = proc.oid.hpid if proc is not None else ''
    tid = evflow.tid if evflow is not None else ''
    opFlags = getOpFlags(evflow.opFlags) if evflow is not None else '' 
    sTs = datetime.fromtimestamp(float(float(evflow.ts)/NANO_TO_SECS)) if evflow is not None else ''
    sTime = sTs.strftime("%m/%d/%YT%H:%M:%S.%f") if evflow is not None else ''
    eTs = datetime.fromtimestamp(float(float(evflow.endTs)/NANO_TO_SECS)) if evflow is not None and not isFileEvent(evflow.opFlags) and not isProcessEvent(evflow.opFlags) else ''
    eTime = eTs.strftime("%m/%d/%YT%H:%M:%S.%f") if eTs != '' else ''
    ret = evflow.ret if evflow is not None and (isFileEvent(evflow.opFlags) or isProcessEvent(evflow.opFlags)) else ''
    res1 = file1.path if file1 is not None else ''
    numBReads = evflow.numRRecvBytes if evflow is not None and not isFileEvent(evflow.opFlags) and not isProcessEvent(evflow.opFlags) else ''
    numBWrites = evflow.numWSendBytes if evflow is not None and not isFileEvent(evflow.opFlags) and not isProcessEvent(evflow.opFlags) else ''
    res2 = file2.path if file2 is not None else ''
    cont = cont.id if cont is not None else '' 
    #print(f'|{exe:50}|{pid:5}|{tid:5}|{opFlags:8}|{sTime:23}|{eTime:23}|{ret:5}|{res1:45}|{numBReads:8}|{numBWrites:8}|{res2:45}|{cont:10}|')
    print("|{0:50}|{1:5}|{2:5}|{3:9}|{4:26}|{5:26}|{6:5}|{7:45}|{8:8}|{9:8}|{10:45}|{11:10}|".format(exe, pid, tid, opFlags, sTime, eTime, ret, res1, numBReads, numBWrites, res2, cont))
