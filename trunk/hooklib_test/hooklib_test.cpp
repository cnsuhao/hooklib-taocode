// hooklib_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../hooklib/hooklib.h"
#include "../hooklib/hooklibcpp.h"
#include "../hooklib/macro_repeat.h"

#pragma comment( lib,"hooklib.lib")


#include "windows.h"


struct _hook_context_ ctx;

int
WINAPI
myMessageBoxA0(
			  __in_opt HWND hWnd,
			  __in_opt LPCSTR lpText,
			  __in_opt LPCSTR lpCaption,
			  __in UINT uType)
{
	return (FUNCTION_UNNAMED_TYPE(int,__stdcall,HWND hwnd,LPCSTR t,LPCSTR ,UINT)&ctx.proxy_function_code)( 0,"test","ok",0);
}
void test0()
{
	int n;
	unsigned char xx[] = { 0xff,0x25,0x0,0x0,0x0 };
	const void** pslot;
	//myMessageBoxA( 0,0,0,0 );
	n=hook_install( &ctx,hook_help_get_function("user32.dll","MessageBoxA"),myMessageBoxA0 );
	if( n<0 )
	{
		pslot = hook_help_alloc_bridgeslot( hook_help_get_modulehandle("user32.dll"),1 );
		if( pslot )
		{

			n=hook_install_by_bridgeslot( &ctx,hook_help_get_function("user32.dll","MessageBoxA"),myMessageBoxA0,pslot );
		}
	}

	//	n=hook_install_by_exportname( &ctx,"user32.dll","MessageBoxA",myMessageBoxA);
	MessageBoxA( 0,0,0,0 );
	hook_uninstall( &ctx );
	hook_help_free_bridgeslot( pslot );
	MessageBoxA( 0,0,0,0 );
	return ;
	
}


THookContext<int>	hook_MessageBox;
CHookBridgeslot	hook_brigdeslot;

int
WINAPI
myMessageBoxA1(
			   __in_opt HWND hWnd,
			   __in_opt LPCSTR lpText,
			   __in_opt LPCSTR lpCaption,
			   __in UINT uType)
{
	
	return hook_MessageBox(hWnd,lpText,"myMessageBoxA1",uType);
}


void test1()
{

	if( false == hook_MessageBox.installByBridgeslot( hook_help_get_function("user32.dll","MessageBoxA"),myMessageBoxA1,hook_brigdeslot ) )return;

	MessageBoxA( 0,0,0,0 );

	hook_MessageBox.unInstall();
	hook_brigdeslot.free();
	MessageBoxA( 0,0,0,0 );
}
THookContext<DWORD> hook_GetTickCount32;
CFilterContext		filter_messagebox;

DWORD WINAPI myGetTicket()
{

	return hook_GetTickCount32()+0xf0000000;
}

void lpfn_filter_messagebox( struct _filter_register_* _register_values_ )
{

	filter_messagebox.skip_rip( _register_values_,1 );
	if( hook_is_x64() )
	{

		printf("x64 old=%s\n",_register_values_->rdx);
		_register_values_->rdx = "filter success,in filter function";
	}else
	{

		void** rsp = filter_messagebox.get_rsp(_register_values_);
		printf("win32 old=%s\n",rsp[2]);
		rsp[2] = "filter success,in filter function";
	}
}

void test_filter()
{

	filter_messagebox.install( hook_help_get_function( "user32.dll","MessageBoxA" ),lpfn_filter_messagebox,&hook_brigdeslot );

	MessageBoxA( 0,"if see this,filter install fail","test_install",0 );

	filter_messagebox.uninstall();

	MessageBoxA( 0,"if not seee this ,filter uninstall fail","test_uninstall",0 );

}

void test_mm()
{


	char* pa = 0;

	memory_ptr("user32.dll");

	byte_ptr p("user32.dll");

	int a = *byte_ptr(byte_ptr("user32.dll")+2);



	return;
}

int _tmain(int argc, _TCHAR* argv[])
{

	test_mm();

	DWORD v;
	if( false == hook_brigdeslot.alloc(hook_help_get_modulehandle("user32.dll"),1) )return 1;
	test_filter();

	install_ws2_all_functions();

	hook_GetTickCount32.installImport( GetModuleHandle(0),"kernel32.dll","GetTickCount",myGetTicket );
	v = GetTickCount();

	socket(0,0,0);
	test0();
	test1();
	return 0;
}

