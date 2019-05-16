#ifndef __SF_OP_FLAGS
#define __SF_OP_FLAGS

/*enum NFOpFlags {
     OP_NF_ACCEPT = ( 1 << 0), 
     OP_NF_CONNECT = (1 << 1),
     OP_NF_SEND   = (1 << 2),
     OP_NF_RECV   = (1 << 3),
     OP_NF_CLOSE  = (1 << 4),
     OP_NF_DELEGATE  = (1 << 5),
     OP_NF_INHERIT  = (1 << 6),
     OP_NF_TRUNCATE  = (1 << 7),
     OP_NF_FINAL  = (1 << 8)
};*/

enum OpFlags {
     OP_CLONE = (1 << 0), 
     OP_EXEC = (1 << 1),
     OP_EXIT = (1 << 2),
     OP_SETUID = (1 << 3),
     OP_SETNS = (1 << 4),
     OP_ACCEPT = (1 << 5), 
     OP_CONNECT = (1 << 6), 
     OP_OPEN = (1 << 7), 
     OP_READ_RECV = (1 << 8),
     OP_WRITE_SEND = (1 << 9), 
     OP_CLOSE = (1 << 10), 
     OP_TRUNCATE = (1 << 11),
     OP_SHUTDOWN = (1 << 12),
     OP_MMAP = (1 << 13),
     OP_DIGEST = (1 << 14), 
     OP_MKDIR = (1 << 15),
     OP_RMDIR = (1 << 16),
     OP_LINK  = (1 << 17),
     OP_UNLINK = (1 << 18), 
     OP_SYMLINK = (1 << 19),
     OP_RENAME = (1 << 20)
};

#define IS_FILE_EVT(FLAG) (FLAG == OP_MKDIR || FLAG == OP_RMDIR || FLAG == OP_LINK || FLAG == OP_UNLINK || FLAG == OP_SYMLINK || FLAG == OP_RENAME) 

#endif 
