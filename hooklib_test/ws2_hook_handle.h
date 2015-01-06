#pragma once

#include "hooklibcpp.h"
#include "winsock2.h"
//socket 创建相关

extern THookContext<SOCKET>	hook_socket;
extern THookContext<SOCKET>	hook_WSASocketW;
extern THookContext<int>	hook_closesocket;
extern THookContext<int>	hook_bind;

//extern THookContext<SOCKET>	hook_listen;
//extern THookContext<SOCKET>	hook_shutdown;

bool		install_ws2_handle_functions(CHookBridgeslot& bridgeslot );


SOCKET WINAPI mysocket(  int af,  int type,  int protocol);
SOCKET WINAPI myWSASocketW(  int af,  int type,  int protocol,  LPWSAPROTOCOL_INFOW lpProtocolInfo,  GROUP g,  DWORD dwFlags);
int WINAPI myclosesocket(SOCKET s);
int WINAPI mybind(  SOCKET s,  const struct sockaddr FAR* name,  int namelen);


int WINAPI mylisten(SOCKET s,int backlog);
int WINAPI myshutdown(SOCKET s,int how);

