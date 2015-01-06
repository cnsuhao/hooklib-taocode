#include "StdAfx.h"
#include "ws2_hook_accept.h"

THookContext<SOCKET>	hook_accept;
THookContext<SOCKET>	hook_WSAAccept;
THookContext<BOOL>	hook_AcceptEx;
THookContext<int>	hook_GetAcceptExSockaddrs;

bool install_ws2_accept_functions(CHookBridgeslot& bridgeslot )
{

	HOOK_BY_NAME(accept);
	HOOK_BY_NAME(WSAAccept);

	//AcceptEx;//这得换一种hook方式
	//GetAcceptExSockaddrs换方式
	return true;
}

SOCKET WINAPI myaccept( SOCKET s,struct sockaddr* addr,int* addrlen )
{
	return hook_accept(s,addr,addrlen);
}

SOCKET WINAPI myWSAAccept( SOCKET s, struct sockaddr FAR* addr, LPINT addrlen, LPCONDITIONPROC lpfnCondition, DWORD dwCallbackData )
{
	return hook_WSAAccept( s,addr,addrlen,lpfnCondition,dwCallbackData );
}

BOOL PASCAL myAcceptEx( SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,LPOVERLAPPED lpOverlapped )
{
	return hook_AcceptEx( sListenSocket,sAcceptSocket,lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,lpdwBytesReceived,lpOverlapped );
}

void PASCAL myGetAcceptExSockaddrs( PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPSOCKADDR* LocalSockaddr,LPINT LocalSockaddrLength,LPSOCKADDR* RemoteSockaddr,LPINT RemoteSockaddrLength )
{
	hook_GetAcceptExSockaddrs( lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,LocalSockaddr,LocalSockaddrLength,RemoteSockaddr,RemoteSockaddrLength );
}
