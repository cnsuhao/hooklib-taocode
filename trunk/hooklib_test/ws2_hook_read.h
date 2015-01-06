#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"

extern THookContext<int>	hook_recv;
extern THookContext<int>	hook_recvfrom;
extern THookContext<int>	hook_WSARecv;
extern THookContext<int>	hook_WSARecvEx;	//Mswsock.dll
extern THookContext<int>	hook_WSARecvFrom;
extern THookContext<int>	hook_WSARecvMsg;

bool		install_ws2_read_functions(CHookBridgeslot& bridgeslot );
bool		install_mswsock_WSARecvEx(CHookBridgeslot& bridgeslot);

int WINAPI myrecv(  SOCKET s,  char FAR* buf,  int len,  int flags);
int WINAPI myrecvfrom(  SOCKET s,  char FAR* buf,  int len,  int flags,  struct sockaddr FAR* from,  int FAR* fromlen);
int WINAPI myWSARecv(  SOCKET s,  LPWSABUF lpBuffers,  DWORD dwBufferCount,  LPDWORD lpNumberOfBytesRecvd,  LPDWORD lpFlags,  LPWSAOVERLAPPED lpOverlapped,  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int PASCAL myWSARecvEx(  SOCKET s,  char * buf,  int len,  int* flags);
int WINAPI myWSARecvFrom(  SOCKET s,  LPWSABUF lpBuffers,  DWORD dwBufferCount,  LPDWORD lpNumberOfBytesRecvd,  LPDWORD lpFlags,  struct sockaddr FAR* lpFrom,  LPINT lpFromlen,  LPWSAOVERLAPPED lpOverlapped,  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI myWSARecvMsg( SOCKET s, LPWSAMSG lpMsg,LPDWORD lpdwNumberOfBytesRecvd,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );

