#include "StdAfx.h"
#include "ws2_hook_connect.h"

THookContext<int>	hook_connect;
THookContext<int>	hook_WSAConnect;
THookContext<BOOL>	hook_ConnectEx;
THookContext<BOOL>	hook_DisconnectEx;
THookContext<SOCKET>	hook_WSAJoinLeaf;

bool install_ws2_connect_functions(CHookBridgeslot& bridgeslot)
{

	HOOK_BY_NAME(connect);
	HOOK_BY_NAME(WSAConnect);
	HOOK_BY_NAME(WSAJoinLeaf);

	//HOOK_WS2_32(ConnectEx);//这得换一种hook方式
	//HOOK_WS2_32(DisconnectEx);//这得换一种hook方式
	
	return true;
}

int WINAPI myconnect( SOCKET s, const struct sockaddr FAR* name, int namelen )
{
	return hook_connect( s,name,namelen );
}

int WINAPI myWSAConnect( SOCKET s, const struct sockaddr FAR* name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS )
{
	return hook_WSAConnect( s,name,namelen,lpCallerData,lpCalleeData,lpSQOS,lpGQOS );
}

BOOL PASCAL myConnectEx( SOCKET s,const struct sockaddr* name,int namelen,PVOID lpSendBuffer,DWORD dwSendDataLength,LPDWORD lpdwBytesSent,LPOVERLAPPED lpOverlapped )
{
	return hook_ConnectEx( s,name,namelen,lpSendBuffer,dwSendDataLength,lpdwBytesSent,lpOverlapped );
}

BOOL PASCAL myDisconnectEx( SOCKET hSocket,LPOVERLAPPED lpOverlapped,DWORD dwFlags,DWORD reserved )
{
	return hook_DisconnectEx( hSocket,lpOverlapped,dwFlags,reserved );
}

SOCKET WINAPI myWSAJoinLeaf( SOCKET s, const struct sockaddr FAR* name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, DWORD dwFlags )
{
	return hook_WSAJoinLeaf( s,name,namelen,lpCallerData,lpCalleeData,lpSQOS,lpGQOS,dwFlags );
}
