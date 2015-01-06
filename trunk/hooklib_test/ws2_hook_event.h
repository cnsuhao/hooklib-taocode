#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"
//socket事件相关函数


extern THookContext<BOOL>	hook_WSAGetOverlappedResult;

bool		install_ws2_event_functions(CHookBridgeslot& bridgeslot );

BOOL WINAPI myWSAGetOverlappedResult(  SOCKET s,  LPWSAOVERLAPPED lpOverlapped,  LPDWORD lpcbTransfer,  BOOL fWait,  LPDWORD lpdwFlags);

//以下函数不需要hook
int WINAPI myselect(	int nfds,fd_set* readfds,	fd_set* writefds,fd_set* exceptfds,	const struct timeval* timeout);
int WINAPI myWSAAsyncSelect(SOCKET s,HWND hWnd,unsigned int wMsg,long lEvent);
int WINAPI myWSAEventSelect( SOCKET s, WSAEVENT hEventObject,  long lNetworkEvents);
int WINAPI myWSAEnumNetworkEvents(SOCKET s,WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
