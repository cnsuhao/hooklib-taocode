#include "StdAfx.h"
#include "hook_all.h"

CHookBridgeslot		m_oHookBridge_ws2_32;
CHookBridgeslot		m_oHookBridge_mswsock;
CHookBridgeslot		m_oHookBridge_iphlpapi;
CHookBridgeslot		m_oHookBridge_kernel32;

bool install_ws2_all_functions()
{
	
	//初始加载ws2_32.dll 这里只调用WSAStartup而不调用 WSACleanup，是为了防止ws2_32被卸载掉，再次被加载，如果再次被加载的话，基址有可能会变动，hook的函数会出现未知错误
	WSADATA wsaData;
	LoadLibraryA( "ws2_32.lib" );
	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) )
	{

		DBG("[!]WSAStartup() fail,LastError=%08x\n",GetLastError() );
		return false;
	}

	if( false == prepareHookBridge( m_oHookBridge_ws2_32,"ws2_32.dll" ) )
	{
		DBG("[!]prepareHookBridge(ws2_32.dll) fail\n" );
		return false;
	}
	if( false == prepareHookBridge( m_oHookBridge_mswsock,"mswsock.dll" ) )
	{
		DBG("[!]prepareHookBridge(mswsock.dll) fail\n" );
		return false;
	}
	if( false == prepareHookBridge( m_oHookBridge_iphlpapi,"iphlpapi.dll" ) )
	{
		DBG("[!]prepareHookBridge(iphlpapi.dll) fail\n" );
		return false;
	}
	if( false == prepareHookBridge( m_oHookBridge_kernel32,"kernel32.dll" ) )
	{
		DBG("[!]prepareHookBridge(kernel32.dll) fail\n" );
		return false;
	}

	install_ws2_accept_functions(m_oHookBridge_ws2_32);
	install_ws2_connect_functions(m_oHookBridge_ws2_32);
	install_ws2_info_functions(m_oHookBridge_ws2_32);
	install_ws2_event_functions(m_oHookBridge_ws2_32);
	install_ws2_handle_functions(m_oHookBridge_ws2_32);
	install_ws2_read_functions(m_oHookBridge_ws2_32);
	install_ws2_write_functions(m_oHookBridge_ws2_32);

	install_mswsock_WSARecvEx(m_oHookBridge_mswsock);

	install_iphlpapi_functions(m_oHookBridge_iphlpapi);

	install_kernel32_functions(m_oHookBridge_kernel32);

	return true;
}

#ifndef _MSWSOCK_
#include "mswsock.h"
#endif
//编译环境 SDK2003 ,将SDK头文件放到最前面

GUID myGuidAcceptEx = WSAID_ACCEPTEX;
GUID myGuidConnectEx = WSAID_CONNECTEX;
GUID myGuidDisconnectEx = WSAID_DISCONNECTEX ;
GUID myGuidTransmitFile = WSAID_TRANSMITFILE;
GUID myGuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
GUID myGuidWSARecvMsg	= WSAID_WSARECVMSG;
GUID myGuidWSASendMsg	= WSAID_WSASENDMSG;


void proxy_ws2_extern_function( const GUID* pGuid,void** pfunctionptr )
{

#define PROXY_EXTERN_FUNCTION( name )\
	if( IsEqualGUID( *pGuid,myGuid##name ) )\
	{\
		if(*pfunctionptr && 0==hook_##name.getProxyFunction() ) hook_##name.setProxyFunction( *pfunctionptr );\
		*pfunctionptr = my##name;\
	}


	PROXY_EXTERN_FUNCTION(AcceptEx)
	else
	PROXY_EXTERN_FUNCTION(ConnectEx)
	else
	PROXY_EXTERN_FUNCTION(DisconnectEx)
	else
	PROXY_EXTERN_FUNCTION(GetAcceptExSockaddrs)
	else
	PROXY_EXTERN_FUNCTION(WSARecvMsg)
	else
	PROXY_EXTERN_FUNCTION(WSASendMsg)

}
