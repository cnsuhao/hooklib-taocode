#pragma once
#include "winsock2.h"
#include "Iphlpapi.h"
#include "hooklibcpp.h"
extern THookContext<ULONG>	hook_GetAdaptersInfo;
extern THookContext<DWORD>	hook_GetIpAddrTable;
extern THookContext<ULONG>	hook_GetAdaptersAddresses;


bool		install_iphlpapi_functions(CHookBridgeslot& bridgeslot);

//Iphlpapi.dll
ULONG WINAPI myGetAdaptersInfo(PIP_ADAPTER_INFO AdapterInfo,OUT PULONG SizePointer);
DWORD WINAPI myGetIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable,PULONG pdwSize,BOOL bOrder);
ULONG WINAPI myGetAdaptersAddresses(ULONG Family, ULONG Flags,PVOID Reserved,PIP_ADAPTER_ADDRESSES AdapterAddresses,PULONG SizePointer);//此函数会调用myGetIpAddrTable,for xlive.dll for 生化危机

