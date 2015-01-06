#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"

extern THookContext<int>	hook_connect;
extern THookContext<int>	hook_WSAConnect;
extern THookContext<BOOL>	hook_ConnectEx;
extern THookContext<BOOL>	hook_DisconnectEx;
extern THookContext<SOCKET>	hook_WSAJoinLeaf;

bool		install_ws2_connect_functions(CHookBridgeslot& bridgeslot );

int WINAPI myconnect(  SOCKET s,  const struct sockaddr FAR* name,  int namelen);
int WINAPI myWSAConnect(  SOCKET s,  const struct sockaddr FAR* name,  int namelen,  LPWSABUF lpCallerData,  LPWSABUF lpCalleeData,  LPQOS lpSQOS,  LPQOS lpGQOS );
BOOL PASCAL myConnectEx(SOCKET s,const struct sockaddr* name,int namelen,PVOID lpSendBuffer,DWORD dwSendDataLength,LPDWORD lpdwBytesSent,LPOVERLAPPED lpOverlapped);
BOOL PASCAL myDisconnectEx(SOCKET hSocket,LPOVERLAPPED lpOverlapped,DWORD dwFlags,DWORD reserved);
SOCKET WINAPI myWSAJoinLeaf(  SOCKET s,  const struct sockaddr FAR* name,  int namelen,  LPWSABUF lpCallerData,  LPWSABUF lpCalleeData,  LPQOS lpSQOS,  LPQOS lpGQOS,  DWORD dwFlags);