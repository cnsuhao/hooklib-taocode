#ifndef _LIBHOOK_
#define _LIBHOOK_

#if defined( HOOKLIB_EXPORTS ) && defined(_MSC_VER)
#define HOOKLIB_API __declspec(dllexport)
#else
#define HOOKLIB_API //__declspec(dllimport)
#endif


#ifndef WINVER                          // ָ��Ҫ������ƽ̨�� Windows Vista��
#define WINVER 0x0600           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0600     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE                       // ָ��Ҫ������ƽ̨�� Internet Explorer 7.0��
#define _WIN32_IE 0x0700        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

#ifdef _WIN64
#define _X64 1
#endif

#define DEF_FUNCTION_NAMED_TYPE( ret_type,call_type,type_name,... ) typedef ret_type (call_type* type_name)(__VA_ARGS__)
#define FUNCTION_UNNAMED_TYPE( ret_type,call_type,... ) (ret_type (call_type*)(__VA_ARGS__))
#define CAST_FUNCTION_POINTER( ret_type,call_type,function,... ) (FUNCTION_UNNAMED_TYPE( ret_type,call_type,__VA_ARGS__ )function)
//DEF_FUNCTION_NAMED_TYPE( int,WINAPI,fnMessageBoxA,HWND hwnd,LPCSTR t,LPCSTR ,UINT );
//(FUNCTION_UNNAMED_TYPE(int,__stdcall,HWND hwnd,LPCSTR t,LPCSTR ,UINT)&ctx.proxy_function_code)( 0,"test","ok",0);


#ifdef __cplusplus
extern "C"
{
#endif

struct _hook_context_
{
	const void*		orginal_function_ptr;//ԭ����,����ԭ������ַ�ĵ�ַ
	int				orginal_code_size;//ԭ���볤��
	unsigned char	orginal_code[32];//ԭ����
	const void*		proxy_function_ptr;
	unsigned char	proxy_function_code[64];//������,ԭ����ͷ���ƹ������º���,���ô˺���������ʵ��ԭ�����Ĺ���
};

//mincopysizeΪ������Ҫ���Ƶ�����,���ƻ����뵽Ŀ���ַ,���ظ��Ƶ��ֽ�����
//�������ֵС��mincopysize����ע���ˣ�һ��������jmpָ��޷���������Ĵ�����
//����from��copy���ֽ�����
//to���һ��ָ��Ϊjmp to from+copysize
HOOKLIB_API int				hook_copy_code( const void* from,int fromsize,void* to,int tosize,int mincopysize );
HOOKLIB_API int				hook_make_jmp( void* buffer,const void* jmpfrom,const void* jmpto );//����buffer��jmp���볤��,ʧ�ܷ���0

//����bool
HOOKLIB_API	int				hook_is_x64();//�Ƿ�64λ 
HOOKLIB_API int				hook_is_readable( const void* memory,size_t size );
HOOKLIB_API int				hook_is_writeable( const void* memory,size_t size );
HOOKLIB_API int				hook_write_memory( const void* address,const void* buffer,int size );//д�����
HOOKLIB_API int				hook_make_executeable( const void* buffer,int size );//ʹ�û�������ִ��
HOOKLIB_API int				hook_make_writeable( const void* buffer,int size );//ʹ�û�������д
HOOKLIB_API	int				hook_install(struct _hook_context_* ctx,const void* function,const void* newfunction );//ʧ�ܷ��ظ���,�����ctx����ȫ����0
HOOKLIB_API	int				hook_install_import(struct _hook_context_* ctx,const void* pmodule,const char* pimport_module_name,const char* pimport_function_name,const void* newfunction );//ʧ�ܷ��ظ���,�����ctx����ȫ����0
HOOKLIB_API	int				hook_install_by_bridgeslot( struct _hook_context_* ctx,const void* function,const void* newfunction,const void** bridgeslot );//ʹ��jmp qword[offset32] ��ת������ֻ��6�ֽ�
HOOKLIB_API	int				hook_uninstall(struct _hook_context_* ctx );
HOOKLIB_API	const void*		hook_help_get_function( const char* modulename,const char* functionname );
HOOKLIB_API	const void*		hook_help_get_function2( const void* pmodule,const char* functionname );
HOOKLIB_API	const void*		hook_help_get_modulehandle( const char* modulename);
HOOKLIB_API const void**	hook_help_alloc_bridgeslot( const void* modulehandle,int maxcount );
HOOKLIB_API void			hook_help_free_bridgeslot( const void**  bridgeslot);




struct _filter_register_
{

	void*	 cflags;
	void*	 rax;
	void*	 rbx;
	void*	 rcx;//x64 argv0
	void*	 rdx;//x64 argv1
	void*	 rbp;
	void*	 rsi;
	void*	 rdi;
#ifdef _X64
	void*	 r8;//x64 argv2
	void*	 r9;//x64 argv3
	void*	 r10;
	void*	 r11;
	void*	 r12;
	void*	 r13;
	void*	 r14;
	void*	 r15;
#endif
};

#define MAX_SKIP_RIP 16
struct _filter_context_
{
	const void*		orginal_code_ptr;
	int				orginal_code_size;//ԭ���볤��
	unsigned char	orginal_code[32];//ԭ����
	
	unsigned char	proxy_code[128];

	int				max_skip_count;
	void*			skip_rip[MAX_SKIP_RIP];
};

typedef void (* lpfn_filter_function)( struct _filter_register_* _register_values_ );

HOOKLIB_API	int		filter_install(struct _filter_context_* ctx,const void* orginal_code_ptr,lpfn_filter_function filter_function,const void** bridgeslot_x64 );//ʧ�ܷ��ظ���,�����ctx����ȫ����0,���bridgeslot=0����ʹ�� jmp bridgeslot[xxx],ֻ��x64��Ч
HOOKLIB_API	int		filter_uninstall(struct _filter_context_* ctx );
HOOKLIB_API	void**	filter_get_rsp( struct _filter_register_* _register_values_ );
HOOKLIB_API	int		filter_skip_rip( struct _filter_context_* ctx,struct _filter_register_* _register_values_,int skip_count );
#ifdef __cplusplus
}
#endif



#endif