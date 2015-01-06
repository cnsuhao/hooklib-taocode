#pragma once

extern THookContext<BOOL>	hook_GetQueuedCompletionStatus;
extern THookContext<DWORD>	hook_GetTickCount;


bool install_kernel32_functions(CHookBridgeslot& bridgeslot);


BOOL WINAPI myGetQueuedCompletionStatus( HANDLE CompletionPort,LPDWORD lpNumberOfBytes,PULONG_PTR lpCompletionKey,LPOVERLAPPED* lpOverlapped,DWORD dwMilliseconds);
DWORD WINAPI myGetTickCount(void);//¸øÓÎÏ·¼õËÙ