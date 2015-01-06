#include "StdAfx.h"
#include "ws2_hook_read.h"
#include "ws2_hook_read.h"

THookContext<int>	hook_recv;
THookContext<int>	hook_recvfrom;
THookContext<int>	hook_WSARecv;
THookContext<int>	hook_WSARecvEx;
THookContext<int>	hook_WSARecvFrom;
THookContext<int>	hook_WSARecvMsg;

CHookBridgeslot		m_oHookBridge_for_wsarecvex;

bool install_ws2_read_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(recv);
	HOOK_BY_NAME(recvfrom);
	HOOK_BY_NAME(WSARecv);
	
	HOOK_BY_NAME(WSARecvFrom);

	//HOOK_WS2_32(WSARecvEx);
	//hook_WSARecvMsg得换一种方式hook
	return true;
}


bool install_mswsock_WSARecvEx(CHookBridgeslot& bridgeslot)
{
	HOOK_BY_NAME(WSARecvEx);

	return true;
}

int WINAPI myrecv( SOCKET s, char FAR* buf, int len, int flags )
{
	return hook_recv( s,buf,len,flags );
}

int WINAPI myrecvfrom( SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen )
{
	return hook_recvfrom( s,buf,len,flags,from,fromlen );
}

int WINAPI myWSARecv( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{

	return hook_WSARecv( s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpOverlapped,lpCompletionRoutine );
}

int PASCAL myWSARecvEx( SOCKET s, char * buf, int len, int* flags )
{
	return hook_WSARecvEx( s,buf,len,flags );
}

int WINAPI myWSARecvFrom( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	return hook_WSARecvFrom( s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine );
}

int WINAPI myWSARecvMsg( SOCKET s, LPWSAMSG lpMsg,LPDWORD lpdwNumberOfBytesRecvd,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	return hook_WSARecvMsg( s,lpMsg,lpdwNumberOfBytesRecvd,lpOverlapped,lpCompletionRoutine );
}
