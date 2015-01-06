#include "StdAfx.h"
#include "kernel32_hook.h"
THookContext<BOOL>	hook_GetQueuedCompletionStatus;
THookContext<DWORD>	hook_GetTickCount;


bool install_kernel32_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(GetQueuedCompletionStatus);
	//HOOK_BY_NAME(GetTickCount);

	return true;
}
BOOL WINAPI myGetQueuedCompletionStatus( HANDLE CompletionPort,LPDWORD lpNumberOfBytes,PULONG_PTR lpCompletionKey,LPOVERLAPPED* lpOverlapped,DWORD dwMilliseconds )
{
	return hook_GetQueuedCompletionStatus( CompletionPort,lpNumberOfBytes,lpCompletionKey,lpOverlapped,dwMilliseconds );
}

DWORD WINAPI myGetTickCount( void )
{
	return hook_GetTickCount();
}
