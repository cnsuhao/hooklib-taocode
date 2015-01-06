#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"


extern THookContext<SOCKET>	hook_accept;
extern THookContext<BOOL>	hook_AcceptEx;

extern THookContext<SOCKET>	hook_WSAAccept;//��WSAIoctl���滻
extern THookContext<int>	hook_GetAcceptExSockaddrs;//��WSAIoctl���滻


bool		install_ws2_accept_functions( CHookBridgeslot& bridgeslot );


SOCKET WINAPI myaccept( SOCKET s,struct sockaddr* addr,int* addrlen );
SOCKET WINAPI myWSAAccept(  SOCKET s,  struct sockaddr FAR* addr,  LPINT addrlen,  LPCONDITIONPROC lpfnCondition,  DWORD dwCallbackData);
BOOL PASCAL myAcceptEx(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,LPOVERLAPPED lpOverlapped);
void PASCAL myGetAcceptExSockaddrs(PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPSOCKADDR* LocalSockaddr,LPINT LocalSockaddrLength,LPSOCKADDR* RemoteSockaddr,LPINT RemoteSockaddrLength);