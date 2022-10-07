/** Copyright (C) 2021 IBM Corporation.
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

#ifndef __SF_MODES_
#define __SF_MODES_
#include <ppm_events_public.h>
#include <unordered_set>

static const std::unordered_set<uint32_t> SF_FLOW_SC_SET{
    PPM_SC_ACCEPT,      PPM_SC_ACCEPT4,   PPM_SC_BIND,      PPM_SC_CHMOD,
    PPM_SC_CLONE,       PPM_SC_CLONE3,    PPM_SC_CLOSE,     PPM_SC_CONNECT,
    PPM_SC_CREAT,       PPM_SC_EXECVE,    PPM_SC_EXECVEAT,  PPM_SC_FORK,
    PPM_SC_LINK,        PPM_SC_LINKAT,    PPM_SC_LISTEN,    PPM_SC_MMAP,
    PPM_SC_MMAP2,       PPM_SC_MKDIR,     PPM_SC_MKDIRAT,   PPM_SC_OPEN,
    PPM_SC_OPENAT,      PPM_SC_OPENAT2,   PPM_SC_PREAD64,   PPM_SC_PREADV,
    PPM_SC_PWRITEV,     PPM_SC_PWRITE64,  PPM_SC_READ,      PPM_SC_READV,
    PPM_SC_RECVFROM,    PPM_SC_RECVMMSG,  PPM_SC_RECVMSG,   PPM_SC_RENAME,
    PPM_SC_RENAMEAT,    PPM_SC_RENAMEAT2, PPM_SC_RMDIR,     PPM_SC_SENDMMSG,
    PPM_SC_SENDMSG,     PPM_SC_SENDTO,    PPM_SC_SETNS,     PPM_SC_SETRESUID,
    PPM_SC_SETRESUID32, PPM_SC_SETUID,    PPM_SC_SETUID32,  PPM_SC_SHUTDOWN,
    PPM_SC_SOCKETPAIR,  PPM_SC_SYMLINK,   PPM_SC_SYMLINKAT, PPM_SC_UNLINK,
    PPM_SC_UNLINKAT,    PPM_SC_VFORK,     PPM_SC_WRITE,     PPM_SC_WRITEV,
};

static const std::unordered_set<uint32_t> SF_CONSUMER_SC_SET{
    PPM_SC_ACCEPT,   PPM_SC_ACCEPT4,    PPM_SC_CHMOD,       PPM_SC_CLONE,
    PPM_SC_CLONE3,   PPM_SC_CONNECT,    PPM_SC_CREAT,       PPM_SC_EXECVE,
    PPM_SC_EXECVEAT, PPM_SC_FORK,       PPM_SC_LINK,        PPM_SC_LINKAT,
    PPM_SC_MKDIR,    PPM_SC_MKDIRAT,    PPM_SC_OPEN,        PPM_SC_OPENAT,
    PPM_SC_OPENAT2,  PPM_SC_RECVMMSG,   PPM_SC_RECVMSG,     PPM_SC_RENAME,
    PPM_SC_RENAMEAT, PPM_SC_RENAMEAT2,  PPM_SC_RMDIR,       PPM_SC_SENDMMSG,
    PPM_SC_SENDMSG,  PPM_SC_SETRESUID,  PPM_SC_SETRESUID32, PPM_SC_SETUID,
    PPM_SC_SETUID32, PPM_SC_SOCKETPAIR, PPM_SC_SYMLINK,     PPM_SC_SYMLINKAT,
    PPM_SC_UNLINK,   PPM_SC_UNLINKAT,   PPM_SC_VFORK,
};
#endif