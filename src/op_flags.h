/** Copyright (C) 2019 IBM Corporation.
*
* Authors:
* Frederico Araujo <frederico.araujo@ibm.com>
* Teryl Taylor <terylt@ibm.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#ifndef __SF_OP_FLAGS
#define __SF_OP_FLAGS

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
