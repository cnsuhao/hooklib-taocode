#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"

//信息相关

extern THookContext<int>	hook_getpeername;
extern THookContext<int>	hook_getsockname;
extern THookContext<int>	hook_ioctlsocket;
extern THookContext<int>	hook_setsockopt;
extern THookContext<int>	hook_getsockopt;
extern THookContext<int>	hook_WSAIoctl;
extern THookContext<int>	hook_gethostname;
extern THookContext<struct hostent* >	hook_gethostbyname;

bool		install_ws2_info_functions(CHookBridgeslot& bridgeslot );

int WINAPI mygetpeername(  SOCKET s,  struct sockaddr FAR* name,  int FAR* namelen);
int WINAPI mygetsockname(  SOCKET s,  struct sockaddr FAR* name,  int FAR* namelen);
int WINAPI myioctlsocket(  SOCKET s,  long cmd,  u_long FAR* argp);
int WINAPI mysetsockopt(  SOCKET s,  int level,  int optname,  const char FAR* optval,  int optlen);
int WINAPI mygetsockopt(SOCKET s,int level,int optname,char* optval,int* optlen);
int WINAPI myWSAIoctl(  SOCKET s,  DWORD dwIoControlCode,  LPVOID lpvInBuffer,  DWORD cbInBuffer,  LPVOID lpvOutBuffer,  DWORD cbOutBuffer,  LPDWORD lpcbBytesReturned,  LPWSAOVERLAPPED lpOverlapped,  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI mygethostname(  char FAR* name,  int namelen);
struct hostent* FAR WINAPI mygethostbyname(  __in          const char* name);

//下面的没有hook
HANDLE WINAPI myWSAAsyncGetHostByName(HWND hWnd,unsigned int wMsg, const char* name,char* buf,int buflen);
