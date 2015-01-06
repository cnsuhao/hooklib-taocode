// x64Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "winsock2.h"
#pragma comment( lib,"ws2_32.lib")

typedef void (*_test)();

void testprintf2( int a,double b )
{

	double d;


	printf("%d,%02f" ,a,b);

	d = b;
}
void testprintf( int a,double b,int c,int d,int e )
{
	double de;

	printf("%d,%02f,%d,%d,%d" ,a,b,c,d,e );

	de = b;

}
int _tmain(int argc, _TCHAR* argv[])
{

	testprintf2(1,2.0);
	testprintf( 1,2.0,3,4,5 );

	int v = 0x12345678;
	__int64  a=0xeFFFFFFFFFffffee;
	char			b = -1;
	b = a;
	a += b;

	//0000000076B212BF 44 39 1D 76 0E 02 00 cmp         dword ptr [76B4213Ch],r11d 
	//testprintf( 1,0.2,3,4,5 );
	unsigned char p[] = {0x90,0x90,  0x44,0x39,0x1d,0x00,0x00,0x00,0x00 };

	DWORD o;
	_test t= (_test)&p;
	VirtualProtect( p,sizeof(p),PAGE_EXECUTE_READWRITE,&o);
	t();
	if( argc < 2 )
	{
	const _test a = (_test)0x1111111111111112;

	((_test)0x1111111111111112)();

	a();
	}

	return 0;
}

