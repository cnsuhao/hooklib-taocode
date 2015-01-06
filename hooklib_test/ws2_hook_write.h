#pragma once

#include "hooklibcpp.h"
#include "winsock2.h"

extern THookContext<int>	hook_send;
extern THookContext<int>	hook_sendto;
extern THookContext<int>	hook_WSASend;
extern THookContext<int>	hook_WSASendTo;
extern THookContext<int>	hook_WSASendMsg;

bool		install_ws2_write_functions(CHookBridgeslot& bridgeslot );

int WINAPI mysend(  SOCKET s,  const char FAR* buf,  int len,  int flags);
int WINAPI mysendto(  SOCKET s,  const char FAR* buf,   int len,  int flags,  const struct sockaddr FAR* to,  int tolen);
int WINAPI myWSASend(  SOCKET s,  LPWSABUF lpBuffers,  DWORD dwBufferCount,  LPDWORD lpNumberOfBytesSent,  DWORD dwFlags,  LPWSAOVERLAPPED lpOverlapped,  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI myWSASendTo(  SOCKET s,  LPWSABUF lpBuffers,  DWORD dwBufferCount,  LPDWORD lpNumberOfBytesSent,  DWORD dwFlags,  const struct sockaddr FAR* lpTo,  int iToLen,  LPWSAOVERLAPPED lpOverlapped,  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI myWSASendMsg(SOCKET s,LPWSAMSG lpMsg,DWORD dwFlags,LPDWORD lpNumberOfBytesSent,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
