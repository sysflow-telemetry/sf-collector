#ifndef __SF_SYS_CALL_DEFS
#define __SF_SYS_CALL_DEFS
#define SF_EXECVE_ENTER() case PPME_SYSCALL_EXECVE_8_E: \
                        case PPME_SYSCALL_EXECVE_13_E: \
                        case PPME_SYSCALL_EXECVE_14_E: \
                        case PPME_SYSCALL_EXECVE_15_E: \
                        case PPME_SYSCALL_EXECVE_16_E: \
                        case PPME_SYSCALL_EXECVE_17_E: \
                        case PPME_SYSCALL_EXECVE_18_E: \
                        case PPME_SYSCALL_EXECVE_19_E: \
                        {                              \
                              break;                   \
                        }

#define SF_EXECVE_EXIT(EV)  case PPME_SYSCALL_EXECVE_8_X: \
                        case PPME_SYSCALL_EXECVE_13_X: \
                        case PPME_SYSCALL_EXECVE_14_X: \
                        case PPME_SYSCALL_EXECVE_15_X: \
                        case PPME_SYSCALL_EXECVE_16_X: \
                        case PPME_SYSCALL_EXECVE_17_X: \
                        case PPME_SYSCALL_EXECVE_18_X: \
                        case PPME_SYSCALL_EXECVE_19_X: \
                        {                              \
			    m_procFlowCxt->writeExecEvent(EV); \
                            break;                     \
                        }

#define SF_CLONE_EXIT(EV)   case PPME_SYSCALL_CLONE_16_X: \
                        case PPME_SYSCALL_CLONE_17_X: \
                        case PPME_SYSCALL_CLONE_20_X: \
                        {                             \
			    m_procFlowCxt->writeCloneEvent(EV); \
                            break;                    \
                        }

#define SF_PROCEXIT_E_X(EV) case PPME_PROCEXIT_E: \
                        case PPME_PROCEXIT_X: \
                        case PPME_PROCEXIT_1_E: \
                        case PPME_PROCEXIT_1_X: \
			  {                     \
			      m_procFlowCxt->writeExitEvent(EV); \
			      break;            \
                          }


#define SF_OPEN_EXIT    case PPME_SYSCALL_OPEN_X:

#define SF_CLOSE_EXIT   case PPME_SYSCALL_CLOSE_X: \
                        case PPME_SOCKET_SHUTDOWN_X: 

#define SF_READ_EXIT    case PPME_SYSCALL_READ_X: \
                        case PPME_SYSCALL_PREAD_X: \
                        case PPME_SYSCALL_PREADV_X: \
                        case PPME_SYSCALL_READV_X: 

#define SF_WRITE_EXIT   case PPME_SYSCALL_WRITE_X: \
                        case PPME_SYSCALL_WRITEV_X: \
                        case PPME_SYSCALL_PWRITEV_X: \
                        case PPME_SYSCALL_PWRITE_X:

#define SF_ACCEPT_EXIT(EV)  case PPME_SOCKET_ACCEPT_X: \
                        case PPME_SOCKET_ACCEPT4_X: \
                        case PPME_SOCKET_ACCEPT_5_X: \
                        case PPME_SOCKET_ACCEPT4_5_X: \
                        case PPME_SYSCALL_SELECT_X:  \
                        case PPM_SC_PSELECT6:     \
			{                     \
			    m_dfCxt->handleDataEvent(EV, OP_NF_ACCEPT); \
			    break;            \
                        }

#define SF_BIND_EXIT    case PPME_SOCKET_BIND_X:

#define SF_CONNECT_EXIT(EV) case PPME_SOCKET_CONNECT_X: \
			{                     \
			    m_dfCxt->handleDataEvent(EV, OP_NF_CONNECT); \
			    break;            \
                        }

#define SF_SEND_EXIT    case PPME_SOCKET_SEND_X: \
                        case PPME_SOCKET_SENDTO_X: \
                        case PPME_SOCKET_SENDMSG_X: \
                        case PPME_SOCKET_SENDMMSG_X: \
                        case PPME_SYSCALL_WRITEV_X: \
                        case PPME_SYSCALL_PWRITEV_X: \
                        case PPME_SYSCALL_PWRITE_X: \
                        case PPME_SYSCALL_WRITE_X: 

#define SF_RECV_EXIT    case PPME_SOCKET_RECV_X: \
                        case PPME_SOCKET_RECVFROM_X: \
                        case PPME_SOCKET_RECVMSG_X: \
                        case PPME_SOCKET_RECVMMSG_X: \
                        case PPME_SYSCALL_PREAD_X: \
                        case PPME_SYSCALL_PREADV_X: \
                        case PPME_SYSCALL_READV_X: \
                        case PPME_SYSCALL_READ_X:

#define SF_SOCKET_PAIR_EXIT case PPME_SOCKET_SOCKETPAIR_X:

#endif
