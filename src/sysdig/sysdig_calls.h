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

#ifndef __SD_SYSDIG_CALL_DEFS
#define __SD_SYSDIG_CALL_DEFS

#define SD_EXECVE_ENTER()                                                      \
  case PPME_SYSCALL_EXECVE_8_E:                                                \
  case PPME_SYSCALL_EXECVE_13_E:                                               \
  case PPME_SYSCALL_EXECVE_14_E:                                               \
  case PPME_SYSCALL_EXECVE_15_E:                                               \
  case PPME_SYSCALL_EXECVE_16_E:                                               \
  case PPME_SYSCALL_EXECVE_17_E:                                               \
  case PPME_SYSCALL_EXECVE_18_E:                                               \
  case PPME_SYSCALL_EXECVE_19_E: {                                             \
    break;                                                                     \
  }

#define SD_EXECVE_EXIT                                                         \
  case PPME_SYSCALL_EXECVE_8_X:                                                \
  case PPME_SYSCALL_EXECVE_13_X:                                               \
  case PPME_SYSCALL_EXECVE_14_X:                                               \
  case PPME_SYSCALL_EXECVE_15_X:                                               \
  case PPME_SYSCALL_EXECVE_16_X:                                               \
  case PPME_SYSCALL_EXECVE_17_X:                                               \
  case PPME_SYSCALL_EXECVE_18_X:                                               \
  case PPME_SYSCALL_EXECVE_19_X: {                                             \
    opFlag = OP_EXEC;                                                          \
    break;                                                                     \
  }

#define SD_CLONE_EXIT                                                          \
  case PPME_SYSCALL_CLONE_11_X:                                                \
  case PPME_SYSCALL_CLONE_16_X:                                                \
  case PPME_SYSCALL_CLONE_17_X:                                                \
  case PPME_SYSCALL_CLONE_20_X:                                                \
  case PPME_SYSCALL_FORK_X:                                                    \
  case PPME_SYSCALL_VFORK_X:                                                   \
  case PPME_SYSCALL_FORK_17_X:                                                 \
  case PPME_SYSCALL_VFORK_17_X:                                                \
  case PPME_SYSCALL_FORK_20_X:                                                 \
  case PPME_SYSCALL_VFORK_20_X: {                                              \
    opFlag = OP_CLONE;                                                         \
    break;                                                                     \
  }

#define SD_PROCEXIT_E_X                                                        \
  case PPME_PROCEXIT_E:                                                        \
  case PPME_PROCEXIT_X:                                                        \
  case PPME_PROCEXIT_1_E:                                                      \
  case PPME_PROCEXIT_1_X: {                                                    \
    opFlag = OP_EXIT;                                                          \
    break;                                                                     \
  }

#define SD_OPEN_EXIT                                                           \
  case PPME_SYSCALL_OPEN_X:                                                    \
  case PPME_SYSCALL_OPENAT_X:                                                  \
  case PPME_SYSCALL_OPENAT_2_X:                                                \
  case PPME_SYSCALL_TIMERFD_CREATE_X: {                                        \
    opFlag = OP_OPEN;                                                          \
    break;                                                                     \
  }

#define SD_CLOSE_EXIT                                                          \
  case PPME_SYSCALL_CLOSE_X: {                                                 \
    opFlag = OP_CLOSE;                                                         \
    break;                                                                     \
  }

#define SD_SHUTDOWN_EXIT                                                       \
  case PPME_SOCKET_SHUTDOWN_X: {                                               \
    opFlag = OP_SHUTDOWN;                                                      \
    break;                                                                     \
  }

#define SD_READ_EXIT                                                           \
  case PPME_SYSCALL_READ_X:                                                    \
  case PPME_SYSCALL_PREAD_X:                                                   \
  case PPME_SYSCALL_PREADV_X:                                                  \
  case PPME_SYSCALL_READV_X:

#define SD_WRITE_EXIT                                                          \
  case PPME_SYSCALL_WRITE_X:                                                   \
  case PPME_SYSCALL_WRITEV_X:                                                  \
  case PPME_SYSCALL_PWRITEV_X:                                                 \
  case PPME_SYSCALL_PWRITE_X:

#define SD_ACCEPT_EXIT                                                         \
  case PPME_SOCKET_ACCEPT_X:                                                   \
  case PPME_SOCKET_ACCEPT4_X:                                                  \
  case PPME_SOCKET_ACCEPT_5_X:                                                 \
  case PPME_SOCKET_ACCEPT4_5_X:                                                \
  case PPME_SYSCALL_SELECT_X:                                                  \
  case PPM_SC_PSELECT6: {                                                      \
    opFlag = OP_ACCEPT;                                                        \
    break;                                                                     \
  }

#define SD_BIND_EXIT case PPME_SOCKET_BIND_X:

#define SD_CONNECT_EXIT                                                        \
  case PPME_SOCKET_CONNECT_X: {                                                \
    opFlag = OP_CONNECT;                                                       \
    break;                                                                     \
  }

#define SD_SEND_EXIT                                                           \
  case PPME_SOCKET_SEND_X:                                                     \
  case PPME_SOCKET_SENDTO_X:                                                   \
  case PPME_SOCKET_SENDMSG_X:                                                  \
  case PPME_SOCKET_SENDMMSG_X:                                                 \
  case PPME_SYSCALL_WRITEV_X:                                                  \
  case PPME_SYSCALL_PWRITEV_X:                                                 \
  case PPME_SYSCALL_PWRITE_X:                                                  \
  case PPME_SYSCALL_WRITE_X: {                                                 \
    opFlag = OP_WRITE_SEND;                                                    \
    break;                                                                     \
  }

#define SD_RECV_EXIT                                                           \
  case PPME_SOCKET_RECV_X:                                                     \
  case PPME_SOCKET_RECVFROM_X:                                                 \
  case PPME_SOCKET_RECVMSG_X:                                                  \
  case PPME_SOCKET_RECVMMSG_X:                                                 \
  case PPME_SYSCALL_PREAD_X:                                                   \
  case PPME_SYSCALL_PREADV_X:                                                  \
  case PPME_SYSCALL_READV_X:                                                   \
  case PPME_SYSCALL_READ_X: {                                                  \
    opFlag = OP_READ_RECV;                                                     \
    break;                                                                     \
  }

#define SD_SOCKET_PAIR_EXIT case PPME_SOCKET_SOCKETPAIR_X:

#define SD_MKDIR_EXIT                                                          \
  case PPME_SYSCALL_MKDIR_X:                                                   \
  case PPME_SYSCALL_MKDIR_2_X:                                                 \
  case PPME_SYSCALL_MKDIRAT_X: {                                               \
    opFlag = OP_MKDIR;                                                         \
    break;                                                                     \
  }

#define SD_RMDIR_EXIT                                                          \
  case PPME_SYSCALL_RMDIR_X:                                                   \
  case PPME_SYSCALL_RMDIR_2_X: {                                               \
    opFlag = OP_RMDIR;                                                         \
    break;                                                                     \
  }

#define SD_LINK_EXIT                                                           \
  case PPME_SYSCALL_LINK_X:                                                    \
  case PPME_SYSCALL_LINK_2_X:                                                  \
  case PPME_SYSCALL_LINKAT_X:                                                  \
  case PPME_SYSCALL_LINKAT_2_X: {                                              \
    opFlag = OP_LINK;                                                          \
    break;                                                                     \
  }

#define SD_UNLINK_EXIT                                                         \
  case PPME_SYSCALL_UNLINK_X:                                                  \
  case PPME_SYSCALL_UNLINK_2_X:                                                \
  case PPME_SYSCALL_UNLINKAT_X:                                                \
  case PPME_SYSCALL_UNLINKAT_2_X: {                                            \
    opFlag = OP_UNLINK;                                                        \
    break;                                                                     \
  }

#define SD_RENAME_EXIT                                                         \
  case PPME_SYSCALL_RENAME_X:                                                  \
  case PPME_SYSCALL_RENAMEAT_X: {                                              \
    opFlag = OP_RENAME;                                                        \
    break;                                                                     \
  }

#define SD_SYMLINK_EXIT                                                        \
  case PPME_SYSCALL_SYMLINK_X:                                                 \
  case PPME_SYSCALL_SYMLINKAT_X: {                                             \
    opFlag = OP_SYMLINK;                                                       \
    break;                                                                     \
  }

#define SD_SETUID_ENTER                                                        \
  case PPME_SYSCALL_SETUID_E:                                                  \
  case PPME_SYSCALL_SETRESUID_E: {                                             \
    opFlag = OP_SETUID;                                                        \
    sysCallEnter = true;                                                       \
    break;                                                                     \
  }

#define SD_SETUID_EXIT                                                         \
  case PPME_SYSCALL_SETUID_X:                                                  \
  case PPME_SYSCALL_SETRESUID_X: {                                             \
    opFlag = OP_SETUID;                                                        \
    break;                                                                     \
  }

#define SD_CHMOD_EXIT                                                          \
  case PPM_SC_CHMOD: {                                                         \
    cout << "GOT A CHMOD" << endl;                                             \
    break;                                                                     \
  }

#define SD_SETNS_EXIT                                                          \
  case PPME_SYSCALL_SETNS_X: {                                                 \
    opFlag = OP_SETNS;                                                         \
    break;                                                                     \
  }

#define SD_MMAP_EXIT                                                           \
  case PPME_SYSCALL_MMAP_E:                                                    \
  case PPME_SYSCALL_MMAP2_E: {                                                 \
    opFlag = OP_MMAP;                                                          \
    break;                                                                     \
  }

#define IS_AT_SC(TYPE)                                                         \
  ((TYPE) == PPME_SYSCALL_SYMLINKAT_X || (TYPE) == PPME_SYSCALL_RENAMEAT_X ||  \
   (TYPE) == PPME_SYSCALL_UNLINKAT_X || (TYPE) == PPME_SYSCALL_UNLINKAT_2_X || \
   (TYPE) == PPME_SYSCALL_LINKAT_X || (TYPE) == PPME_SYSCALL_LINKAT_2_X ||     \
   (TYPE) == PPME_SYSCALL_MKDIRAT_X)

#define IS_UNLINKAT(TYPE)                                                      \
  ((TYPE) == PPME_SYSCALL_UNLINKAT_X || (TYPE) == PPME_SYSCALL_UNLINKAT_2_X)
#endif
