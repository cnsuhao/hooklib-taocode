#include "StdAfx.h"
#include "iphlpapi_hook.h"


THookContext<ULONG>	hook_GetAdaptersInfo;
THookContext<DWORD>	hook_GetIpAddrTable;
THookContext<ULONG>	hook_GetAdaptersAddresses;

CHookBridgeslot		bridgeslot;

bool install_iphlpapi_functions(CHookBridgeslot& bridgeslot)
{
	
	HOOK_BY_NAME(GetAdaptersInfo);
	HOOK_BY_NAME(GetIpAddrTable);
	HOOK_BY_NAME(GetAdaptersAddresses);

	return true;
}

ULONG WINAPI myGetAdaptersInfo( PIP_ADAPTER_INFO AdapterInfo,OUT PULONG SizePointer )
{
	return hook_GetAdaptersInfo( AdapterInfo,SizePointer );
}

DWORD WINAPI myGetIpAddrTable( PMIB_IPADDRTABLE pIpAddrTable,PULONG pdwSize,BOOL bOrder )
{
	return hook_GetIpAddrTable( pIpAddrTable,pdwSize,bOrder );
}

ULONG WINAPI myGetAdaptersAddresses( ULONG Family, ULONG Flags,PVOID Reserved,PIP_ADAPTER_ADDRESSES AdapterAddresses,PULONG SizePointer )
{
	return hook_GetAdaptersAddresses( Family,Flags,Reserved,AdapterAddresses,SizePointer );
}
