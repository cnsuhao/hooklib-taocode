#include "StdAfx.h"
#include "ws2_hook_info.h"



THookContext<int>	hook_getpeername;
THookContext<int>	hook_getsockname;
THookContext<int>	hook_ioctlsocket;
THookContext<int>	hook_setsockopt;
THookContext<int>	hook_getsockopt;
THookContext<int>	hook_WSAIoctl;
THookContext<int>	hook_gethostname;
THookContext<struct hostent* >	hook_gethostbyname;

bool install_ws2_info_functions(CHookBridgeslot& bridgeslot)
{	

	HOOK_BY_NAME(getpeername);
	HOOK_BY_NAME(getsockname);
	HOOK_BY_NAME(ioctlsocket);
	HOOK_BY_NAME(setsockopt);
	HOOK_BY_NAME(getsockopt);
	HOOK_BY_NAME(WSAIoctl);
	HOOK_BY_NAME(gethostname);
	HOOK_BY_NAME(gethostbyname);
	return true;
}

int WINAPI mygetpeername( SOCKET s, struct sockaddr FAR* name, int FAR* namelen )
{
	return hook_getpeername( s,name,namelen );
}

int WINAPI mygetsockname( SOCKET s, struct sockaddr FAR* name, int FAR* namelen )
{
	return hook_getsockname( s,name,namelen );
}

int WINAPI myioctlsocket( SOCKET s, long cmd, u_long FAR* argp )
{
	return hook_ioctlsocket( s,cmd,argp );
}

int WINAPI mysetsockopt( SOCKET s, int level, int optname, const char FAR* optval, int optlen )
{
	return hook_setsockopt( s,level,optname,optval,optlen );
}

int WINAPI mygetsockopt( SOCKET s,int level,int optname,char* optval,int* optlen )
{
	return hook_getsockopt( s,level,optname,optval,optlen );
	
}

int WINAPI myWSAIoctl( SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
	int n;
	
	n = hook_WSAIoctl( s,dwIoControlCode,lpvInBuffer,cbInBuffer,lpvOutBuffer,cbOutBuffer,lpcbBytesReturned,lpOverlapped,lpCompletionRoutine );
	if( SIO_GET_EXTENSION_FUNCTION_POINTER == dwIoControlCode && SOCKET_ERROR != n && cbInBuffer >= sizeof(GUID) && cbOutBuffer >= sizeof(void*) )
	{
		proxy_ws2_extern_function( (const GUID*)lpvInBuffer,(void**)lpvOutBuffer );
	}

	return n;
}

int WINAPI mygethostname( char FAR* name, int namelen )
{
	return hook_gethostname( name,namelen );
}

struct hostent* FAR WINAPI mygethostbyname( __in const char* name )
{
	return hook_gethostbyname( name );
}
