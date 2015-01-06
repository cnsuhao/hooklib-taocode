#include "StdAfx.h"
#include "ws2_hook_event.h"

THookContext<BOOL>	hook_WSAGetOverlappedResult;

bool install_ws2_event_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(WSAGetOverlappedResult);

	return true;
}

BOOL WINAPI myWSAGetOverlappedResult( SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags )
{
	return hook_WSAGetOverlappedResult( s,lpOverlapped,lpcbTransfer,fWait,lpdwFlags );
}
