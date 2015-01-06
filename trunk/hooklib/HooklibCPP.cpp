#include "HooklibCPP.h"
extern "C"
{
#include "port_win32.h"
}
CHookBridgeslot::CHookBridgeslot()
{
	bridgeslot = 0;
	m_modulehandle = 0;
}

CHookBridgeslot::~CHookBridgeslot()
{

}

bool CHookBridgeslot::alloc( const void* modulehandle,int maxcount )
{
	m_modulehandle = modulehandle;
	bridgeslot = hook_help_alloc_bridgeslot( modulehandle,maxcount );
	return 0!=bridgeslot;
}



void CHookBridgeslot::free()
{
	if(bridgeslot)hook_help_free_bridgeslot(bridgeslot);
}

const void* CHookBridgeslot::getModuleHandle()
{
	return m_modulehandle;
}

bool CHookBridgeslot::isValid()
{
	return 0!=bridgeslot;
}

void CHookBridgeslot::setModuleHandle( const void* modulehandle )
{
	m_modulehandle = modulehandle;
}


CHookContext::CHookContext()
{
	memset( &ctx,0,sizeof(ctx) );
}


CHookContext::~CHookContext()
{

}
const void* CHookContext::getProxyFunction()
{
	return ctx.proxy_function_ptr;
}

void CHookContext::setProxyFunction( const void* function )
{
	ctx.proxy_function_ptr = function;
}

bool CHookContext::install( const void* function,const void* newfunction )
{
	int n;
	n = hook_install( &ctx,function,newfunction );
	return n>=0;
}


bool CHookContext::installImport( const void* pmodule,const char* pimport_module_name,const char* pimport_function_name,const void* newfunction )
{
	int n;
	n = hook_install_import( &ctx,pmodule,pimport_module_name,pimport_function_name,newfunction );
	return n>=0;
}

bool CHookContext::installByBridgeslot( const void* function,const void* newfunction,const CHookBridgeslot& bridgeslot )
{
	int n;
	n = hook_install_by_bridgeslot( &ctx,function,newfunction,bridgeslot.bridgeslot );
	return n>=0;
}


void CHookContext::unInstall()
{
	hook_uninstall(&ctx);
}


bool CHookContext::isValid()
{
	return 0!=ctx.proxy_function_ptr;
}




bool prepareHookBridge(CHookBridgeslot& modulebridge,const char* modulename)//最多支持0x1000/sizeof(void*)-1个函数
{
	const void* pmodule;

	pmodule = hook_help_get_modulehandle(modulename);
	if( 0 == pmodule )return false;

	if( hook_is_x64() )
	{
		if( false == modulebridge.alloc( pmodule,(0x1000/sizeof(void*))-1 ) )
		{
			return false;
		}
	}else
	{
		modulebridge.setModuleHandle( pmodule );
	}
	return true;
}

bool	hookModuleFunction( CHookBridgeslot& modulebridge,CHookContext* phook_ctx,const void* function,const void* newfunction,bool disable_jmp_bridge )
{
	bool bsuccess = true;
	if( 0 == function )return false;
	do 
	{
		if( disable_jmp_bridge )
		{
			if( true == phook_ctx->install( function,newfunction ) )break;
		}else if( modulebridge.isValid() )
		{
			if( true == phook_ctx->installByBridgeslot( function,newfunction,modulebridge ) )break;
		}else
		{
			if( true == phook_ctx->install( function,newfunction ) )break;
		}

		bsuccess = false;
	} while (0);
	return bsuccess;
}



bool	hookModuleFunction2( CHookBridgeslot& modulebridge,CHookContext* phook_ctx,const char* functionname,const void* newfunction,bool disable_jmp_bridge )
{
	const void*	pfunction;
	pfunction = hook_help_get_function2( modulebridge.getModuleHandle(),functionname);
	if( 0 == pfunction )return false;
	return hookModuleFunction( modulebridge,phook_ctx,pfunction,newfunction,disable_jmp_bridge);
}


CFilterContext::CFilterContext()
{

	memset( &ctx,0,sizeof(ctx) );
}

CFilterContext::~CFilterContext()
{

}

bool CFilterContext::install( const void* orgin_ptr,lpfn_filter_function filter_function,CHookBridgeslot* slot /*= 0 */ )
{
	return 0 == filter_install( &ctx,orgin_ptr,filter_function,slot?slot->bridgeslot:0 );
}

bool CFilterContext::is_valid()
{
	if( 0 == ctx.orginal_code_ptr || ctx.orginal_code_size < 1 )return false;

	return true;
}

void CFilterContext::uninstall()
{

	filter_uninstall( &ctx );
}

void** CFilterContext::get_rsp( struct _filter_register_* _register_values_ )
{

	return filter_get_rsp( _register_values_ );
}

bool CFilterContext::skip_rip( struct _filter_register_* _register_values_,int skip_count )
{
	return 0!= filter_skip_rip( &ctx,_register_values_,skip_count );
}

memory_ptr::memory_ptr( mm_address_type _address /*= mm_invalid_address*/ )
{
	set_address( _address );
}

memory_ptr::memory_ptr( const char* module_name,bool load_if_not_exits )
{
	const void* p = port_help_get_modulehandle(module_name,load_if_not_exits );

	set_address( (mm_address_type)p );
}

memory_ptr::memory_ptr( const void* _address )
{
	set_address( (mm_address_type)_address );
}

/*
memory_ptr::memory_ptr( size_t _address  )
{
	set_address( (mm_address_type)_address );
}
*/
void memory_ptr::set_address( mm_address_type _address /*= mm_invalid_address*/ )
{
	mm_address = _address;
}

mm_address_type memory_ptr::get_address() const
{
	return mm_address;
}

bool memory_ptr::is_readable( int size ) const
{
	return 0 == port_is_bad_read_ptr( (const void*)mm_address,size );
}

bool memory_ptr::is_writeable( int size ) const
{
	return 0 == port_is_bad_write_ptr( (const void*)mm_address,size );
}

memory_ptr memory_ptr::add( int bytes ) const
{
	return memory_ptr( (mm_address_type)((const char*)mm_address+bytes));
}

const memory_ptr& memory_ptr::add_self( int bytes )
{
	mm_address = (mm_address_type)((const char*)mm_address+bytes);
	return *this;
}

memory_ptr memory_ptr::sub( int bytes ) const
{
	return memory_ptr((mm_address_type) ((const char*)mm_address-bytes));
}

const memory_ptr& memory_ptr::sub_self( int bytes )
{
	mm_address = (mm_address_type)((const char*)mm_address-bytes);
	return *this;
}

memory_ptr::operator mm_address_type() const
{
	return get_address();
}

memory_ptr memory_ptr::operator*() const
{

	if( !is_readable(sizeof(void*)))return memory_ptr();

	return memory_ptr( (mm_address_type)*((void**)mm_address) );
}

memory_ptr memory_ptr::operator+( int bytes ) const
{
	return add(bytes);
}

memory_ptr memory_ptr::operator-( int bytes ) const
{

	return sub(bytes);
}
