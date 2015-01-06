#pragma once

#include "hooklib.h"
#include "macro_repeat.h"

class HOOKLIB_API CHookBridgeslot
{
public:
	CHookBridgeslot();
	~CHookBridgeslot();

	bool	alloc(const void* modulehandle,int maxcount);
	void	free();

	const void* getModuleHandle();
	void		setModuleHandle(const void* modulehandle);
	bool	isValid();

	const void** bridgeslot;
private:
	const void*	m_modulehandle;
};

class HOOKLIB_API CFilterContext
{
public:
	CFilterContext();
	~CFilterContext();
	
	bool				install( const void* orgin_ptr,lpfn_filter_function filter_function,CHookBridgeslot* slot = 0 );
	bool				is_valid();
	void				uninstall();

	static void**		get_rsp(struct _filter_register_* _register_values_);
	bool				skip_rip( struct _filter_register_* _register_values_,int skip_count );
private:
	struct _filter_context_	ctx;
};

class HOOKLIB_API CHookContext
{
public:
	CHookContext();
	~CHookContext();
	bool				install( const void* function,const void* newfunction );
	bool				installImport( const void* pmodule,const char* pimport_module_name,const char* pimport_function_name,const void* newfunction );
	bool				installByBridgeslot( const void* function,const void* newfunction,const CHookBridgeslot& bridgeslot );
	void				unInstall();
	bool				isValid();

	const void*			getProxyFunction();
	void				setProxyFunction( const void* function );
private:
	struct _hook_context_ ctx;
};

typedef enum
{
	HOOK_CALL_TYPE_CDECL,//__cdecl c语言调用方式 
	HOOK_CALL_TYPE_STDCALL,//winapi 调用方式
	HOOK_CALL_TYPE_PASCALL,//pascall
}HOOK_CALL_TYPE;

template<typename ret_type=void*,HOOK_CALL_TYPE call_type=HOOK_CALL_TYPE_STDCALL>
class THookContext:public CHookContext
{
public:

	ret_type			emit();
	ret_type			operator()();
/*
	template< typename _argv0 >
	ret_type			emit( _argv0 argv0 );

	template< typename _argv0 >
	ret_type operator()( _argv0 argv0 )
*/
	//下面的宏，要复制制作 15个emit和operator,参数最多15个，如需要更多，自己弄去

#define __typename_param(n, ...) typename _argv##n,
#define __typename_end(n, ...) typename _argv##n
#define repeat_typename(n, ...) __repeat(n,__typename_param,__typename_end)

#define __type_param(n, ...)  _argv##n argv##n,
#define __type_param_end(n, ...)  _argv##n argv##n
#define repeat_type(n, ...) __repeat(n,__type_param,__type_param_end)

#define __argv_param(n, ...)  argv##n,
#define __argv_param_end(n, ...)  argv##n
#define repeat_argv(n, ...) __repeat(n,__argv_param,__argv_param_end)

#define repeat_emit(n, ...)\
	template< repeat_typename(n,__VA_ARGS__) >\
	ret_type			emit( repeat_type(n,__VA_ARGS__) )\
	{\
		const void*	function = getProxyFunction();\
		if(function)\
		switch( call_type )\
		{\
			case  HOOK_CALL_TYPE_CDECL:\
				return CAST_FUNCTION_POINTER( ret_type,__cdecl,function,repeat_type(n,__VA_ARGS__) )(repeat_argv(n,__VA_ARGS__));\
			case  HOOK_CALL_TYPE_STDCALL:\
				return CAST_FUNCTION_POINTER( ret_type,__stdcall,function,repeat_type(n,__VA_ARGS__) )(repeat_argv(n,__VA_ARGS__));\
			case  HOOK_CALL_TYPE_PASCALL:\
				return CAST_FUNCTION_POINTER( ret_type,pascal,function,repeat_type(n,__VA_ARGS__) )(repeat_argv(n,__VA_ARGS__));\
		}\
		return 0;\
	}

#define repeat_operator(n, ...)\
	template< repeat_typename(n,__VA_ARGS__) >\
	ret_type operator()( repeat_type(n,__VA_ARGS__))\
	{\
		return emit(repeat_argv(n,__VA_ARGS__));\
	}

#define __for_each_number_begin_with_1(v, ...) \
	v(1, __VA_ARGS__) \
	v(2, __VA_ARGS__) \
	v(3, __VA_ARGS__) \
	v(4, __VA_ARGS__) \
	v(5, __VA_ARGS__) \
	v(6, __VA_ARGS__) \
	v(7, __VA_ARGS__) \
	v(8, __VA_ARGS__) \
	v(9, __VA_ARGS__) \
	v(10, __VA_ARGS__) \
	v(11, __VA_ARGS__) \
	v(12, __VA_ARGS__) \
	v(13, __VA_ARGS__) \
	v(14, __VA_ARGS__) \
	v(15, __VA_ARGS__)

__for_each_number_begin_with_1(repeat_emit);
__for_each_number_begin_with_1(repeat_operator);
};

//偏特化,虽然没有必要，还是写上吧

template<HOOK_CALL_TYPE call_type>
class THookContext<void,call_type>:public CHookContext
{
public:
	void			emit();

};

HOOKLIB_API bool	prepareHookBridge(CHookBridgeslot& modulebridge,const char* modulename);//最多支持0x1000/sizeof(void*)-1个函数
HOOKLIB_API bool	hookModuleFunction2( CHookBridgeslot& modulebridge,CHookContext* phook_ctx,const char* functionname,const void* newfunction,bool disable_jmp_bridge );
HOOKLIB_API bool	hookModuleFunction( CHookBridgeslot& modulebridge,CHookContext* phook_ctx,const void* function,const void* newfunction,bool disable_jmp_bridge );


#ifdef _X64
#define mm_address_type	unsigned __int64
#else
#define mm_address_type	unsigned __int32
#endif


//#define mm_address_type void*
#define mm_invalid_address 0



class HOOKLIB_API memory_ptr
{
public:
	memory_ptr( const char* module_name,bool load_if_not_exits = true );
	memory_ptr( mm_address_type _address = mm_invalid_address );
	//memory_ptr( size_t _address  );
	memory_ptr( const void* _address );
	void						set_address(mm_address_type _address = mm_invalid_address );
	mm_address_type				get_address() const;
	bool						is_readable( int size ) const;
	bool						is_writeable( int size ) const;
	memory_ptr					add( int bytes ) const;
	memory_ptr					sub( int bytes ) const;
	const memory_ptr&			add_self( int bytes );
	const memory_ptr&			sub_self( int bytes );

	operator					mm_address_type() const;
	memory_ptr					operator*() const;
	memory_ptr					operator+( int bytes ) const;
	memory_ptr					operator-( int bytes ) const;
private:
	mm_address_type				mm_address;
};

template<typename _type>
class HOOKLIB_API value_visitor_t;

template<typename _type>
class HOOKLIB_API type_ptr_t:public memory_ptr
{
public:
	type_ptr_t( const char* module_name,bool load_if_not_exits = true );
	type_ptr_t( mm_address_type _address = mm_invalid_address );
	type_ptr_t( const memory_ptr& ptr );
	//type_ptr_t( size_t _address  );
	type_ptr_t( const void* _address ):memory_ptr(_address){};

	bool					is_readable(  ) const{return memory_ptr::is_readable(sizeof(_type));}
	bool					is_writeable(  ) const{return memory_ptr::is_writeable(sizeof(_type));}

	type_ptr_t&				operator++();//++p
	type_ptr_t&				operator--();//--p
	const type_ptr_t		operator++(int);//p++
	const type_ptr_t		operator--(int);//p--
	type_ptr_t				operator+( int type_count ) const;//p+2
	type_ptr_t				operator-( int type_count ) const;//p-1
	const type_ptr_t&		operator+=( int type_count );//p+=2
	const type_ptr_t&		operator-=( int type_count );//p-=2

	value_visitor_t<_type>	operator*() const;
	value_visitor_t<_type>	operator[](int index);
};



typedef type_ptr_t<unsigned __int8> byte_ptr;
typedef type_ptr_t<unsigned __int16> word_ptr;
typedef type_ptr_t<unsigned __int32> dword_ptr;
typedef type_ptr_t<unsigned __int64> qword_ptr;
typedef type_ptr_t<mm_address_type> address_ptr;

template<typename _type>
class HOOKLIB_API value_visitor_t
{
public:
	value_visitor_t( mm_address_type _address );

	void				set_address( mm_address_type _address );
	mm_address_type		get_address() const;

	bool				set_value( _type value );
	_type				get_value() const;
	bool				add_value( _type value );
	bool				sub_value( _type value );
	bool				is_readable() const;
	bool				is_writeable() const;

	bool						operator=(_type value);
	operator					const _type() const;

	value_visitor_t&			operator++();//++p
	value_visitor_t&			operator--();//--p
	const value_visitor_t		operator++(int);//p++
	const value_visitor_t		operator--(int);//p--
	_type						operator+( _type value ) const;//p+value
	_type						operator-( _type value ) const;//p-value
	const value_visitor_t&		operator+=( _type value );//p+=value
	const value_visitor_t&		operator-=( _type value );//p-=value

	template<typename _casttype>
	operator _casttype() const{return (_casttype)get_value();}
private:
	_type*						value_address;
};



#include "value_visitor.inl"


#include "callemit.h"