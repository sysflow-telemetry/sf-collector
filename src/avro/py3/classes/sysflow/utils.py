import sysflow.opflags as opflags
from datetime import datetime

NANO_TO_SECS = 1000000000
TIME_FORMAT = "%m/%d/%YT%H:%M:%S.%f"

def getOpFlagsStr(opFlags):
    ops = ""
    ops +=  "MKDIR" if (opFlags & opflags.OP_MKDIR) else "";
    ops +=  "RMDIR" if (opFlags & opflags.OP_RMDIR) else "";
    ops +=  "LINK" if (opFlags & opflags.OP_LINK) else "";
    ops +=  "SYMLINK" if (opFlags & opflags.OP_SYMLINK) else  "";
    ops +=  "UNLINK" if (opFlags & opflags.OP_UNLINK) else  "";
    ops +=  "RENAME" if (opFlags & opflags.OP_RENAME) else  "";

    if(len(ops) > 0):
        return ops
    
    ops +=  "CLONE" if (opFlags & opflags.OP_CLONE) else "";
    ops +=  "EXEC" if (opFlags & opflags.OP_EXEC) else "";
    ops +=  "EXIT" if (opFlags & opflags.OP_EXIT) else "";
    ops +=  "SETUID" if (opFlags & opflags.OP_SETUID) else  "";
    
    if(len(ops) > 0):
        return ops

    ops +=  "O" if (opFlags & opflags.OP_OPEN) else  " ";
    ops +=  "A" if (opFlags & opflags.OP_ACCEPT) else " ";
    ops +=  "C" if (opFlags & opflags.OP_CONNECT) else  " ";
    ops +=  "W" if (opFlags & opflags.OP_WRITE_SEND)  else " ";
    ops +=  "R" if (opFlags & opflags.OP_READ_RECV)  else " ";
    ops +=  "N" if (opFlags & opflags.OP_SETNS)  else " ";
    ops +=  "M" if (opFlags & opflags.OP_MMAP)  else " ";
    ops +=  "S" if (opFlags & opflags.OP_SHUTDOWN)  else " ";
    ops +=  "C" if (opFlags & opflags.OP_CLOSE)  else " ";
    ops +=  "T" if (opFlags & opflags.OP_TRUNCATE) else " ";
    ops +=  "D" if (opFlags & opflags.OP_DIGEST)  else " ";
    return ops

def getTimeStr(ts):
    tStamp = datetime.fromtimestamp(float(float(ts)/NANO_TO_SECS))
    timeStr = tStamp.strftime(TIME_FORMAT)
    return timeStr

def getNetFlowStr(nf):
    sip = ".".join(map(lambda n: str(nf.sip>>n & 0xFF), [0, 8, 16, 24]))
    dip = ".".join(map(lambda n: str(nf.dip>>n & 0xFF), [0, 8, 16, 24]))
    return str(sip) + ":" + str(nf.sport) + "-" + str(dip) + ":" + str(nf.dport)  

