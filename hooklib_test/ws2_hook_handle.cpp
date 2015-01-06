#include "StdAfx.h"
#include "ws2_hook_handle.h"


THookContext<SOCKET>	hook_socket;
THookContext<SOCKET>	hook_WSASocketW;
THookContext<int>		hook_closesocket;
THookContext<int>		hook_bind;

bool install_ws2_handle_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(socket);
	HOOK_BY_NAME(WSASocketW);
	HOOK_BY_NAME(closesocket);
	HOOK_BY_NAME(bind);
	return true;
}

SOCKET WINAPI mysocket( int af, int type, int protocol )
{
	return hook_socket( af,type,protocol);
}

SOCKET WINAPI myWSASocketW( int af, int type, int protocol, LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags )
{
	return hook_WSASocketW(af,type,protocol,lpProtocolInfo,g,dwFlags );
}

int WINAPI myclosesocket( SOCKET s )
{
	return hook_closesocket(s);
}

int WINAPI mybind( SOCKET s, const struct sockaddr FAR* name, int namelen )
{
	return hook_bind( s,name,namelen );
}
