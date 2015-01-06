#pragma once
#include "hooklibcpp.h"
#include "winsock2.h"



#include "ws2_hook_accept.h"
#include "ws2_hook_connect.h"
#include "ws2_hook_info.h"
#include "ws2_hook_event.h"
#include "ws2_hook_handle.h"
#include "ws2_hook_read.h"
#include "ws2_hook_write.h"
#include "iphlpapi_hook.h"
#include "kernel32_hook.h"

#ifndef DBG
#ifdef _DEBUG
#define DBG(...) printf(__FUNCTION__":" __VA_ARGS__);
#else
#define DBG
#endif
#endif

#define DISABLE_JMP_BY_BRIDGE 0

bool		install_ws2_all_functions();
void		proxy_ws2_extern_function(const GUID* pGuid,void** pfunctionptr );//for myWSAIoctl

#define HOOK_BY_NAME( name )\
	if( false == hookModuleFunction2( bridgeslot,&hook_##name,#name,my##name,DISABLE_JMP_BY_BRIDGE ) )\
	{\
		DBG( "[!]hook(%s,0x%p) fail\n",#name,hook_help_get_function2(bridgeslot.getModuleHandle(),#name) );\
	}else\
	{\
		DBG( "hook(%s,0x%p) success\n",#name,hook_help_get_function2(bridgeslot.getModuleHandle(),#name) );\
	}