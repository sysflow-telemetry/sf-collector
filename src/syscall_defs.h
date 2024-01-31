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

#define SF_EXECVE_EXIT(EV)                                                     \
  case OP_EXEC: {                                                              \
    m_ctrlPrcr->handleProcEvent(EV);                                           \
    break;                                                                     \
  }

#define SF_CLONE_EXIT(EV)                                                      \
  case OP_CLONE: {                                                             \
    m_ctrlPrcr->handleProcEvent(EV);                                           \
    break;                                                                     \
  }

#define SF_PROCEXIT_E_X(EV)                                                    \
  case OP_EXIT: {                                                              \
    m_ctrlPrcr->handleProcEvent(EV);                                           \
    break;                                                                     \
  }

#define SF_OPEN_EXIT(EV)                                                       \
  case OP_LOAD_LIBRARY:                                                        \
  case OP_OPEN: {                                                              \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_CLOSE_EXIT(EV)                                                      \
  case OP_CLOSE: {                                                             \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_SHUTDOWN_EXIT(EV)                                                   \
  case OP_SHUTDOWN: {                                                          \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_ACCEPT_EXIT(EV)                                                     \
  case OP_ACCEPT: {                                                            \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_CONNECT_EXIT(EV)                                                    \
  case OP_CONNECT: {                                                           \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_SEND_EXIT(EV)                                                       \
  case OP_WRITE_SEND: {                                                        \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_RECV_EXIT(EV)                                                       \
  case OP_READ_RECV: {                                                         \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_MKDIR_EXIT(EV)                                                      \
  case OP_MKDIR: {                                                             \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_RMDIR_EXIT(EV)                                                      \
  case OP_RMDIR: {                                                             \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_LINK_EXIT(EV)                                                       \
  case OP_LINK: {                                                              \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_UNLINK_EXIT(EV)                                                     \
  case OP_UNLINK: {                                                            \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_RENAME_EXIT(EV)                                                     \
  case OP_RENAME: {                                                            \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_SYMLINK_EXIT(EV)                                                    \
  case OP_SYMLINK: {                                                           \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_SETUID(EV)                                                          \
  case OP_SETUID: {                                                            \
    if (EV->sysCallEnter) {                                                    \
      m_ctrlPrcr->setUID(EV);                                                  \
    } else {                                                                   \
      m_ctrlPrcr->handleProcEvent(EV);                                         \
    }                                                                          \
    break;                                                                     \
  }

#define SF_CHMOD_EXIT(EV)                                                      \
  case PPM_SC_CHMOD: {                                                         \
    cout << "GOT A CHMOD" << endl;                                             \
    break;                                                                     \
  }

#define SF_SETNS_EXIT(EV)                                                      \
  case OP_SETNS: {                                                             \
    m_dfPrcr->handleDataEvent(EV);                                             \
    break;                                                                     \
  }

#define SF_MMAP_EXIT(EV)                                                       \
  case OP_MMAP: {                                                              \
    m_dfPrcr->handleDataEvent(EV);                                             \
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
