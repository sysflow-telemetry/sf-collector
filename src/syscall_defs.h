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

#ifndef __SF_SYS_CALL_DEFS
#define __SF_SYS_CALL_DEFS

#define SF_EXECVE_ENTER()                                                      \
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

#define SF_EXECVE_EXIT(EV)                                                     \
  case PPME_SYSCALL_EXECVE_8_X:                                                \
  case PPME_SYSCALL_EXECVE_13_X:                                               \
  case PPME_SYSCALL_EXECVE_14_X:                                               \
  case PPME_SYSCALL_EXECVE_15_X:                                               \
  case PPME_SYSCALL_EXECVE_16_X:                                               \
  case PPME_SYSCALL_EXECVE_17_X:                                               \
  case PPME_SYSCALL_EXECVE_18_X:                                               \
  case PPME_SYSCALL_EXECVE_19_X: {                                             \
    m_procEvtPrcr->writeExecEvent(EV);                                         \
    break;                                                                     \
  }

#define SF_CLONE_EXIT(EV)                                                      \
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
    m_procEvtPrcr->writeCloneEvent(EV);                                        \
    break;                                                                     \
  }

#define SF_PROCEXIT_E_X(EV)                                                    \
  case PPME_PROCEXIT_E:                                                        \
  case PPME_PROCEXIT_X:                                                        \
  case PPME_PROCEXIT_1_E:                                                      \
  case PPME_PROCEXIT_1_X: {                                                    \
    m_procEvtPrcr->writeExitEvent(EV);                                         \
    break;                                                                     \
  }

#define SF_OPEN_EXIT(EV)                                                       \
  case PPME_SYSCALL_OPEN_X:                                                    \
  case PPME_SYSCALL_OPENAT_X:                                                  \
  case PPME_SYSCALL_OPENAT_2_X:                                                \
  case PPME_SYSCALL_TIMERFD_CREATE_X: {                                        \
    m_dfPrcr->handleDataEvent(EV, OP_OPEN);                                    \
    break;                                                                     \
  }

#define SF_CLOSE_EXIT(EV)                                                      \
  case PPME_SYSCALL_CLOSE_X: {                                                 \
    m_dfPrcr->handleDataEvent(EV, OP_CLOSE);                                   \
    break;                                                                     \
  }

#define SF_SHUTDOWN_EXIT(EV)                                                   \
  case PPME_SOCKET_SHUTDOWN_X: {                                               \
    m_dfPrcr->handleDataEvent(EV, OP_SHUTDOWN);                                \
    break;                                                                     \
  }

#define SF_READ_EXIT                                                           \
  case PPME_SYSCALL_READ_X:                                                    \
  case PPME_SYSCALL_PREAD_X:                                                   \
  case PPME_SYSCALL_PREADV_X:                                                  \
  case PPME_SYSCALL_READV_X:

#define SF_WRITE_EXIT                                                          \
  case PPME_SYSCALL_WRITE_X:                                                   \
  case PPME_SYSCALL_WRITEV_X:                                                  \
  case PPME_SYSCALL_PWRITEV_X:                                                 \
  case PPME_SYSCALL_PWRITE_X:

#define SF_ACCEPT_EXIT(EV)                                                     \
  case PPME_SOCKET_ACCEPT_X:                                                   \
  case PPME_SOCKET_ACCEPT4_X:                                                  \
  case PPME_SOCKET_ACCEPT_5_X:                                                 \
  case PPME_SOCKET_ACCEPT4_5_X:                                                \
  case PPME_SYSCALL_SELECT_X:                                                  \
  case PPM_SC_PSELECT6: {                                                      \
    m_dfPrcr->handleDataEvent(EV, OP_ACCEPT);                                  \
    break;                                                                     \
  }

#define SF_BIND_EXIT case PPME_SOCKET_BIND_X:

#define SF_CONNECT_EXIT(EV)                                                    \
  case PPME_SOCKET_CONNECT_X: {                                                \
    m_dfPrcr->handleDataEvent(EV, OP_CONNECT);                                 \
    break;                                                                     \
  }

#define SF_SEND_EXIT(EV)                                                       \
  case PPME_SOCKET_SEND_X:                                                     \
  case PPME_SOCKET_SENDTO_X:                                                   \
  case PPME_SOCKET_SENDMSG_X:                                                  \
  case PPME_SOCKET_SENDMMSG_X:                                                 \
  case PPME_SYSCALL_WRITEV_X:                                                  \
  case PPME_SYSCALL_PWRITEV_X:                                                 \
  case PPME_SYSCALL_PWRITE_X:                                                  \
  case PPME_SYSCALL_WRITE_X: {                                                 \
    m_dfPrcr->handleDataEvent(EV, OP_WRITE_SEND);                              \
    break;                                                                     \
  }

#define SF_RECV_EXIT(EV)                                                       \
  case PPME_SOCKET_RECV_X:                                                     \
  case PPME_SOCKET_RECVFROM_X:                                                 \
  case PPME_SOCKET_RECVMSG_X:                                                  \
  case PPME_SOCKET_RECVMMSG_X:                                                 \
  case PPME_SYSCALL_PREAD_X:                                                   \
  case PPME_SYSCALL_PREADV_X:                                                  \
  case PPME_SYSCALL_READV_X:                                                   \
  case PPME_SYSCALL_READ_X: {                                                  \
    m_dfPrcr->handleDataEvent(EV, OP_READ_RECV);                               \
    break;                                                                     \
  }

#define SF_SOCKET_PAIR_EXIT case PPME_SOCKET_SOCKETPAIR_X:

#define SF_MKDIR_EXIT(EV)                                                      \
  case PPME_SYSCALL_MKDIR_X:                                                   \
  case PPME_SYSCALL_MKDIR_2_X:                                                 \
  case PPME_SYSCALL_MKDIRAT_X: {                                               \
    m_dfPrcr->handleDataEvent(EV, OP_MKDIR);                                   \
    break;                                                                     \
  }

#define SF_RMDIR_EXIT(EV)                                                      \
  case PPME_SYSCALL_RMDIR_X:                                                   \
  case PPME_SYSCALL_RMDIR_2_X: {                                               \
    m_dfPrcr->handleDataEvent(EV, OP_RMDIR);                                   \
    break;                                                                     \
  }

#define SF_LINK_EXIT(EV)                                                       \
  case PPME_SYSCALL_LINK_X:                                                    \
  case PPME_SYSCALL_LINK_2_X:                                                  \
  case PPME_SYSCALL_LINKAT_X:                                                  \
  case PPME_SYSCALL_LINKAT_2_X: {                                              \
    m_dfPrcr->handleDataEvent(EV, OP_LINK);                                    \
    break;                                                                     \
  }

#define SF_UNLINK_EXIT(EV)                                                     \
  case PPME_SYSCALL_UNLINK_X:                                                  \
  case PPME_SYSCALL_UNLINK_2_X:                                                \
  case PPME_SYSCALL_UNLINKAT_X:                                                \
  case PPME_SYSCALL_UNLINKAT_2_X: {                                            \
    m_dfPrcr->handleDataEvent(EV, OP_UNLINK);                                  \
    break;                                                                     \
  }

#define SF_RENAME_EXIT(EV)                                                     \
  case PPME_SYSCALL_RENAME_X:                                                  \
  case PPME_SYSCALL_RENAMEAT_X: {                                              \
    m_dfPrcr->handleDataEvent(EV, OP_RENAME);                                  \
    break;                                                                     \
  }

#define SF_SYMLINK_EXIT(EV)                                                    \
  case PPME_SYSCALL_SYMLINK_X:                                                 \
  case PPME_SYSCALL_SYMLINKAT_X: {                                             \
    m_dfPrcr->handleDataEvent(EV, OP_SYMLINK);                                 \
    break;                                                                     \
  }

#define SF_SETUID_ENTER(EV)                                                    \
  case PPME_SYSCALL_SETUID_E:                                                  \
  case PPME_SYSCALL_SETRESUID_E: {                                             \
    m_procEvtPrcr->setUID(EV);                                                 \
    break;                                                                     \
  }

#define SF_SETUID_EXIT(EV)                                                     \
  case PPME_SYSCALL_SETUID_X:                                                  \
  case PPME_SYSCALL_SETRESUID_X: {                                             \
    m_procEvtPrcr->writeSetUIDEvent(EV);                                       \
    break;                                                                     \
  }

#define SF_CHMOD_EXIT(EV)                                                      \
  case PPM_SC_CHMOD: {                                                         \
    cout << "GOT A CHMOD" << endl;                                             \
    break;                                                                     \
  }

#define SF_SETNS_EXIT(EV)                                                      \
  case PPME_SYSCALL_SETNS_X: {                                                 \
    m_dfPrcr->handleDataEvent(EV, OP_SETNS);                                   \
    break;                                                                     \
  }

#define SF_MMAP_EXIT(EV)                                                       \
  case PPME_SYSCALL_MMAP_E:                                                    \
  case PPME_SYSCALL_MMAP2_E: {                                                 \
    m_dfPrcr->handleDataEvent(EV, OP_MMAP);                                    \
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
