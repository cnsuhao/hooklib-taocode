#include "StdAfx.h"
#include "ws2_hook_write.h"

THookContext<int>	hook_send;
THookContext<int>	hook_sendto;
THookContext<int>	hook_WSASend;
THookContext<int>	hook_WSASendTo;
THookContext<int>	hook_WSASendMsg;

bool install_ws2_write_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(send);
	HOOK_BY_NAME(sendto);
	HOOK_BY_NAME(WSASend);
	HOOK_BY_NAME(WSASendTo);

	//hook_WSASendMsg得换一种方式hook
	return true;
}

int WINAPI mysend( SOCKET s, const char FAR* buf, int len, int flags )
{
	return hook_send( s,buf,len,flags );
}

int WINAPI mysendto( SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen )
{
	return hook_sendto( s,buf,len,flags,to,tolen );
}

int WINAPI myWSASend( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	return hook_WSASend( s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,lpOverlapped,lpCompletionRoutine );
}

int WINAPI myWSASendTo( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr FAR* lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	return hook_WSASendTo( s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,lpTo,iToLen,lpOverlapped,lpCompletionRoutine );
}

int WINAPI myWSASendMsg( SOCKET s,LPWSAMSG lpMsg,DWORD dwFlags,LPDWORD lpNumberOfBytesSent,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	return hook_WSASendMsg( s,lpMsg,dwFlags,lpNumberOfBytesSent,lpOverlapped,lpCompletionRoutine );
}
