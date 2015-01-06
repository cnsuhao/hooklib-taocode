#include "HookLib.h"
#define SUPPORT_64BIT_OFFSET
#include <distorm.h>
#include <mnemonics.h>
#include <stdio.h>
#include "port_win32.h"
#define IsX64 ( sizeof(void*) == 8 )

	//x86 ia32需要处理如下指令,这些指令兼容64位,但得注意，offset为有符号数,寻址范围不超过4G
	/*
	inst:call rip+offset
	hex:e8 offset( 4 bytes )
	dest:rip+5+offset

	inst:jmp rip+offset
	hex:e9 offset( 4 bytes )
	des:rip+5+offset

	inst:jmp short rip+offset
	hex:eb offset( 1 bytes )
	des:rip+2+offset

	inst:jmpc  rip+offset
	hex:0f [80-8f] offset( 4 bytes )
	des:rip+5+offset

	inst:jmpc short rip+offset
	hex:[70-7f] offset( 1 bytes )
	dest:rip+2+offset

	inst:jecxz short rip+offset
	hex:e3 offset( 1 bytes )
	dest:rip+2+offset

	inst:loopxxx short rip+offset loop指令最大寻址范围256
	hex:[e0-e2] offset( 1 bytes )
	dest:rip+2+offset

	amd64(类 op qword[offset32] )
	inst:jmp qword[offset32]	//与ia32不同
	hex:ff25 offset32
	dest:qword [rip+6+offset32]

	inst:call qword[offset32] //与ia32不同
	hex:ff15 offset32
	dest:qword [rip+6+offset32]

	//0000000076B212BF 44 39 1D 76 0E 02 00 cmp         dword ptr [76B4213Ch],r11d 
	inst: cmp dword ptr[offset32],r11d
	hex:44 39 1D offset32
	dest:qword [rip+7+offset32]

	amd64需要处理的，太多了，暂时就处理到这吧

	jmp代码

	jmp qword[address]
	ff25 00000000 xx xx xx xx xx xx xx xx 14字节
	*/


//O_PC jmp rip+xxxx
//O_SMEN jmp [rip+xxxx]

int isneedadjust( _DInst* inst)
{
	int i;
	for (i = 0; ((i < OPERANDS_NO) && (inst->ops[i].type != O_NONE)); i++)
	{
		if( O_SMEM == inst->ops[i].type )
		{
			if( R_RIP == inst->ops[i].index )
			{
				return 1;
			}
		}
	}
	return O_PC == inst->ops[0].type;
}

void dumpInst( const _CodeInfo* codeInfo,const _DInst* inst )
{
	_DecodedInst	decodedInstructions;

	distorm_format( codeInfo,inst,&decodedInstructions );
	printf("%08I64x (%02d) %-24s %s%s%s\r\n", decodedInstructions.offset, decodedInstructions.size, (char*)decodedInstructions.instructionHex.p, (char*)decodedInstructions.mnemonic.p, decodedInstructions.operands.length != 0 ? " " : "", (char*)decodedInstructions.operands.p);
	
}
uint64_t abs64( int64_t a )
{
	return a>=0?a:-a;
}
uint32_t abs32( int32_t a )
{
	return a>=0?a:-a;
}

unsigned char*	make_jmp_qword_disp( unsigned char* jmpbuffer,const unsigned char* jmpfrom,const unsigned char* jmp_disp_address )
{
	if( IsX64 && ( abs64(jmp_disp_address-jmpfrom-6) > 0x000000007FFFFFFF ) )
	{
		return jmpbuffer;
	}

	*jmpbuffer++ = 0xff;
	*jmpbuffer++ = 0x25;
	if( IsX64 )
	{
		*(uint32_t*)jmpbuffer = (uint32_t)(jmp_disp_address-jmpfrom-6);
	}else
	{
		*(uint32_t*)jmpbuffer = (uint32_t)jmp_disp_address;
	}
	jmpbuffer += 4;
	return jmpbuffer;
}
unsigned char*	make_jmp_short( unsigned char* jmpbuffer,const unsigned char* jmpfrom,const unsigned char* jmpto,unsigned char op_code_short )
{
	*jmpbuffer++ = op_code_short;
	*jmpbuffer++ = (uint8_t)(jmpto-jmpfrom-2);
	return jmpbuffer;
}
/*make_jmp_near
x86:
0xe9 xxxxxxx
jmp/call xxxxxxx+eip+5

x64 jmp:
jmp qword [0];
qd	jmpto


x64 call:
call qword[2];
jmp offset L8	//eb 08,跳过下面8字节数据
qd jmpto	/8字节
L8:
*/
unsigned char*	make_jmp_near( unsigned char* jmpbuffer,const unsigned char* jmpfrom,const unsigned char* jmpto,int iscall )//jmpto为目标地址,iscall=0时为jmp
{
	if( IsX64 && ( abs64(jmpto-jmpfrom-5) > 0x000000007FFFFFFF ) )
	{

		//jmp qword[next instructs]
		*jmpbuffer++ = 0xff;
		*jmpbuffer++ = iscall?0x15:0x25;//0x15为call
		

		*jmpbuffer++ = 0;
		*jmpbuffer++ = 0;
		*jmpbuffer++ = 0;
		*jmpbuffer++ = iscall?2:0;

		/*
		jmp:
		jmp qword [0];
		qd	jmpto	

		call:
		call qword[2];
		jmp offset L8	//eb 08,跳过下面8字节数据
		qd jmpto	/8字节
		L8:

		*/
		if(iscall)
		{
			*jmpbuffer++ = 0xeb;
			*jmpbuffer++ = 0x08;
		}

		*(uint64_t*)(jmpbuffer) = (uint64_t)jmpto;
		jmpbuffer += 8;

		
	}else if( 0 == iscall && (abs32((int32_t)(jmpto-jmpfrom-5)) < 0x00000080 ) )//jmp short
	{
		jmpbuffer = make_jmp_short( jmpbuffer,jmpfrom,jmpto,0xeb );
	}else
	{
		*jmpbuffer++ = iscall?0xe8:0xe9;

		*(int32_t*)jmpbuffer = (int32_t)(jmpto-jmpfrom-5);
		jmpbuffer += 4;
	}
	return jmpbuffer;
}

//jmpfrom为发起跳转的地址
//jmp_condition_to为条件满足时，跳转的地址
//jmp_uncondtion_to为条件不满足时，跳转地址

unsigned char*	make_jmpc_near( unsigned char* jmpbuffer,const unsigned char* jmpfrom,const unsigned char* jmp_condition_to,const unsigned char* jmp_uncondtion_to,unsigned char code0,unsigned char code1 )//jmp condtion,code0,code1指定跳转指令前两字节
{
	unsigned char* p_jmpc_short;

	if( IsX64 && ( abs64(jmp_condition_to-jmpfrom-5) > 0x000000007FFFFFFF ) )
	{

		/*
		
		jmpc short L_to_condition
		jmp jmp_uncondtion_to
L_to_condition:
		jmp jmp_condition_to
		*/

		
		if( 0x0f == code0 )
		{
			code0 = code1-0x10;
		}

		p_jmpc_short = jmpbuffer;
		jmpbuffer+=2;
		jmpbuffer = make_jmp_near( jmpbuffer,jmpbuffer,jmp_uncondtion_to,0 );	//jmp jmp_uncondtion_to
					make_jmp_short( p_jmpc_short,p_jmpc_short,jmpbuffer,code0 );//jmpc short L_to_condition
		jmpbuffer = make_jmp_near( jmpbuffer,jmpbuffer,jmp_condition_to,0 );	//jmp jmp_condition_to

	}else
	{
		*jmpbuffer++ = code0;
		if( 0x0f == code0 )//jmpc near offset32
		{
			*jmpbuffer++ = code1;
		}else//jmpc short offet8,转成jmpc near offset32
		{
			*jmpbuffer++ = code1+0x10;
		}
		
		*(int32_t*)jmpbuffer = (int32_t)(jmp_condition_to-jmpfrom-5);
		jmpbuffer += 4;

		jmpbuffer = make_jmp_near( jmpbuffer,jmpbuffer,jmp_uncondtion_to,0 );
	}
	return jmpbuffer;
}

#define get_dest_address_0xe9( pos ) ((char*)pos+5+*(int32_t*)((char*)pos+1))//jmp near
#define get_dest_address_0xeb( pos ) ((char*)pos+2+*(int8_t*)((char*)pos+1))//jmp short

//不需要考虑lock前缀，因为使用相对偏移的指令，不能lock
//hook后必须FlushInstructionCache
HOOKLIB_API int	 hook_copy_code( const void* from,int fromsize,void* to,int tosize,int mincopysize )
{
	_CodeInfo		codeInfo = {0};
	_DInst			inst={0};
	_DecodeResult	result;
	unsigned int	instCount = 0;
	unsigned char*	pfrom = (unsigned char*)from;
	unsigned char*	pfrom_end = pfrom+fromsize;
	unsigned char*	pto = (unsigned char*)to;
	unsigned char*	p = 0;
	unsigned char*	pto_end = pto+tosize;

	while( pfrom < pfrom_end && pto<pto_end  )
	{
		codeInfo.code		= (const uint8_t*)pfrom;
		codeInfo.codeLen	= (int)(pfrom_end-pfrom);
		codeInfo.codeOffset = (_OffsetType)pfrom;
		codeInfo.features	= DF_NONE;
		codeInfo.dt			= IsX64?Decode64Bits:Decode32Bits;

		instCount = 0;
		result = distorm_decompose( &codeInfo,&inst,1,&instCount );
		if( DECRES_SUCCESS != result && instCount != 1 )return 0;

		//dumpInst( &codeInfo,&inst );
		//isneedadjust(&inst);

		if( 0xe9 == pfrom[0] //jmp offset32,
			|| 0xe8 == pfrom[0] )//call offset32
		{
			pto = make_jmp_near( pto,pto,get_dest_address_0xe9(pfrom),0xe8 == pfrom[0] );
			if(0xe9 == pfrom[0])//call
			{
				break;
			}

		}else if( 0xeb == pfrom[0] )//jmp offset8,
		{
			pto = make_jmp_near( pto,pto,get_dest_address_0xeb(pfrom),0 );
			break;

		}else if( 0x0f == pfrom[0] && (pfrom[1] >= 0x80 && pfrom[1]<=0x8f) )// jmpc offset32
		{
			pto = make_jmpc_near( pto,pto,(pfrom+6+*(int32_t*)(pfrom+2)),(pfrom+6),pfrom[0],pfrom[1] );
			break;
		}else if(pfrom[0] >= 0x70 && pfrom[0]<=0x7f)  //jmpc offset8
		{
			pto = make_jmpc_near( pto,pto,(pfrom+2+*(int8_t*)(pfrom+1)),(pfrom+2),pfrom[0],pfrom[1] );
			break;
		}else if( pfrom[0] >= 0xe0 && pfrom[0] <= 0xe3 )//loop offset8,if( 0xe3 == pfrom[0] )//jecxz offset8 这种情况没法处理,因为没有对应的loop/jecxz offset32指令
		{
			break;
		}else if( isneedadjust(&inst) )
		{
			inst.size = 0;
			break;
		}else
		{
			memcpy( pto,pfrom,inst.size );
			pto += inst.size;
		}
		
		if( pfrom+inst.size >= (unsigned char*)from+mincopysize )break;

		pfrom += inst.size;
	}
	pfrom += inst.size;

	if( pfrom < pfrom_end && pto<pto_end  )
	{
		pto = make_jmp_near( pto,pto,pfrom,0 );
	}

	return (int)(pfrom-(unsigned char*)from);
}

HOOKLIB_API int  hook_make_jmp( void* buffer,const void* jmpfrom,const void* jmpto )
{
	unsigned char* p;
	unsigned char* pend;

	p = (unsigned char*)buffer;
	pend = make_jmp_near( p,(const unsigned char*)jmpfrom,(const unsigned char*)jmpto,0 );

	return (int)(pend-p);
}

HOOKLIB_API	int hook_install( struct _hook_context_* ctx,const void* function,const void* newfunction )
{
	unsigned char	jmpcode[24] = {0};
	int				jmpcodelen = 0;

	if( 0 == ctx || 0 == function || 0 == newfunction )return -1;
	if(ctx->orginal_code_size>0)return 1;
	memset( ctx,0,sizeof(struct _hook_context_) );

	ctx->orginal_function_ptr = function;

	jmpcodelen = hook_make_jmp( jmpcode,function,newfunction );

	hook_make_executeable(ctx->proxy_function_code,sizeof(ctx->proxy_function_code));
	ctx->proxy_function_ptr = (const void*)&ctx->proxy_function_code ;

	ctx->orginal_code_size = hook_copy_code( function,100,ctx->proxy_function_code,sizeof(ctx->proxy_function_code),jmpcodelen );
	if( ctx->orginal_code_size < jmpcodelen )
	{
		memset( ctx,0,sizeof(struct _hook_context_) );
		return -1;
	}

	memcpy( ctx->orginal_code,function,ctx->orginal_code_size );

	hook_write_memory( function,jmpcode,jmpcodelen );

	return 0;
}

HOOKLIB_API	int hook_install_import( struct _hook_context_* ctx,const void* pmodule,const char* pimport_module_name,const char* pimport_function_name,const void* newfunction )
{
	const void** import_slot;

	if( 0 == ctx || 0 == pmodule || 0 == pimport_module_name || 0 == pimport_function_name || 0 == newfunction )return -1;
	if(ctx->orginal_code_size>0)return 1;
	memset( ctx,0,sizeof(struct _hook_context_) );

	import_slot = port_get_import_slot( pmodule,pimport_module_name,pimport_function_name );
	if( 0 == import_slot )return -1;

	ctx->orginal_function_ptr = (const void*)import_slot;
	ctx->orginal_code_size = sizeof(void*);
	memcpy( ctx->orginal_code,ctx->orginal_function_ptr,ctx->orginal_code_size );
	ctx->proxy_function_ptr = *import_slot;
	hook_write_memory( import_slot,&newfunction,sizeof(void*) );

	return 0;
}

HOOKLIB_API	int hook_install_by_bridgeslot( struct _hook_context_* ctx,const void* function,const void* newfunction,const void** bridgeslot )
{
	unsigned char	jmpcode[24] = {0};
	int				jmpcodelen = 0;
	unsigned char*	p;
	const void*		orignal_function = function;
	const void**	bridge_slot = (void**)bridgeslot;

	if( 0 == ctx || 0 == bridgeslot || 0 == function || 0 == newfunction )return -1;
	if(ctx->orginal_code_size>0)return 1;
	memset( ctx,0,sizeof(struct _hook_context_) );

	(*(int*)bridge_slot) ++;//最前面一个元素，存放当前数量,真正存放指针，从第2个元素开始
	bridge_slot += *(int*)bridge_slot;

	//jmp qword[offset32]
	p = make_jmp_qword_disp( jmpcode,orignal_function,(const unsigned char*)bridge_slot );
	jmpcodelen = (int)(p-jmpcode);
	if( 0 == jmpcodelen )return -1;

	ctx->orginal_function_ptr = orignal_function;

	//复制原始函数头字节到proxy_function_code
	hook_make_executeable(ctx->proxy_function_code,sizeof(ctx->proxy_function_code));
	ctx->proxy_function_ptr = (const void*)&ctx->proxy_function_code;
	ctx->orginal_code_size = hook_copy_code( orignal_function,100,ctx->proxy_function_code,sizeof(ctx->proxy_function_code),jmpcodelen );
	if( ctx->orginal_code_size < jmpcodelen )
	{
		memset( ctx,0,sizeof(struct _hook_context_) );
		return -1;
	}

	//保存头字节
	memcpy( ctx->orginal_code,orignal_function,ctx->orginal_code_size );

	*bridge_slot = newfunction;
	hook_write_memory( orignal_function,jmpcode,jmpcodelen );
	return 0;
}

HOOKLIB_API	int hook_uninstall( struct _hook_context_* ctx )
{
	int n;
	if( 0 == ctx )return -1;
	if( 0 == ctx->orginal_function_ptr || ctx->orginal_code_size < 1)return -1;


	n = hook_write_memory( ctx->orginal_function_ptr,ctx->orginal_code,ctx->orginal_code_size );

	memset( ctx,0,sizeof(struct _hook_context_) );
	
	return n;
}

HOOKLIB_API int hook_write_memory( const void* address,const void* buffer,int size )
{
	return 0==port_write_memory( address,buffer,size );
}

HOOKLIB_API int hook_make_executeable( const void* buffer,int size )
{
	return 0==port_make_executeable( buffer,size );
}

HOOKLIB_API int hook_make_writeable( const void* buffer,int size )
{
	return 0==port_make_writeable(buffer,size);
}

HOOKLIB_API	const void* hook_help_get_function( const char* modulename,const char* functionname )
{
	return port_help_get_function( modulename,functionname );
}

HOOKLIB_API const void** hook_help_alloc_bridgeslot( const void* modulehandle,int maxcount )
{
	unsigned int i;
	const void** p =0;
	const char*  pmodule;
	int			size = max( (maxcount+1)*sizeof(void*),0x1000);
	int			modulesize;

	modulesize = port_get_module_size(modulehandle);
	if( 0 == modulesize )return 0;
	
	pmodule = (const char*)modulehandle;
	pmodule -= ((uint32_t)pmodule & 0xfff );
	for( i = 0;i<0x8ffff000/0x1000;++i)
	{
		pmodule -= size;
		p = (const void**)port_alloc_fix_address( pmodule,size );
		if( p )
		{
			p[0] = 0;
			return p;
		}
	}

	pmodule = (const char*)modulehandle;
	pmodule += modulesize+0x1000;
	pmodule += ((uint32_t)pmodule & 0xfff );	
	for( i = 0;i<0x8ffff000/0x1000;++i)
	{
		pmodule += size;
		p = (const void**)port_alloc_fix_address( pmodule,size );
		if( p )
		{
			p[0] = 0;
			return p;
		}
	}
	return 0;
}

HOOKLIB_API void hook_help_free_bridgeslot( const void** bridgeslot )
{
	port_free_fix_address( bridgeslot );
}

HOOKLIB_API	const void* hook_help_get_modulehandle( const char* modulename )
{
	return port_help_get_modulehandle( modulename,1 );
}

HOOKLIB_API	int hook_is_x64()
{
	return (int)IsX64;
}

HOOKLIB_API	const void* hook_help_get_function2( const void* pmodule,const char* functionname )
{
	return port_help_get_function2(pmodule,functionname );
}

int make_skip_array( struct _filter_context_* ctx,const char* proxy_code_ptr,int max_proxy_code,const char* orginal_code_ptr,int max_orginal_code )
{
	_CodeInfo		codeInfo = {0};
	_DInst			inst={0};
	_DecodeResult	result;
	unsigned int	instCount = 0;
	const unsigned char*	pfrom = (const unsigned char*)proxy_code_ptr;
	const unsigned char*	pfrom_end = proxy_code_ptr+max_proxy_code;

	ctx->max_skip_count = 0;

	while( pfrom < pfrom_end && ctx->max_skip_count < MAX_SKIP_RIP )
	{
		codeInfo.code		= (const uint8_t*)pfrom;
		codeInfo.codeLen	= (int)(pfrom_end-pfrom);
		codeInfo.codeOffset = (_OffsetType)pfrom;
		codeInfo.features	= DF_NONE;
		codeInfo.dt			= IsX64?Decode64Bits:Decode32Bits;

		instCount = 0;
		result = distorm_decompose( &codeInfo,&inst,1,&instCount );
		if( DECRES_SUCCESS != result && instCount != 1 )return ctx->max_skip_count>0;

		if( I_JMP_FAR == inst.opcode || I_JMP == inst.opcode )break;
		ctx->skip_rip[ctx->max_skip_count++] = (void*)inst.addr;
		pfrom += inst.size;
	}

	pfrom = (unsigned char*)orginal_code_ptr;
	pfrom_end = orginal_code_ptr+max_orginal_code;

	while( pfrom < pfrom_end && ctx->max_skip_count < MAX_SKIP_RIP )
	{
		codeInfo.code		= (const uint8_t*)pfrom;
		codeInfo.codeLen	= (int)(pfrom_end-pfrom);
		codeInfo.codeOffset = (_OffsetType)pfrom;
		codeInfo.features	= DF_NONE;
		codeInfo.dt			= IsX64?Decode64Bits:Decode32Bits;

		instCount = 0;
		result = distorm_decompose( &codeInfo,&inst,1,&instCount );
		if( DECRES_SUCCESS != result && instCount != 1 )return ctx->max_skip_count>0;

		
		ctx->skip_rip[ctx->max_skip_count++] = (void*)inst.addr;
		pfrom += inst.size;
		if( I_RET == inst.opcode || I_RETF == inst.opcode )break;
	}

	return ctx->max_skip_count>0;
}

int filter_install_x86( struct _filter_context_* ctx,const void* orginal_code_ptr,lpfn_filter_function filter_function )
{
	unsigned char* p;
	unsigned char	jmpcode[24] = {0};
	int				jmpcodelen = 0;

	memset( jmpcode,0x90,sizeof(jmpcode));
	memset( ctx,0,sizeof(struct _filter_context_) );

	jmpcodelen = make_jmp_near( jmpcode,orginal_code_ptr,ctx->proxy_code,0 )-jmpcode;
	if( 0 == jmpcodelen )return 0;

	ctx->orginal_code_ptr = orginal_code_ptr;
	
	p = ctx->proxy_code;

	/*x86
	CPU Disasm
	00401000  /$ /EB 1F         JMP SHORT 00401021
	00401002  |. |57            PUSH EDI
	00401003  |. |56            PUSH ESI
	00401004  |. |55            PUSH EBP
	00401005  |. |52            PUSH EDX
	00401006  |. |51            PUSH ECX
	00401007  |. |53            PUSH EBX
	00401008  |. |50            PUSH EAX
	00401009  |? |9C            PUSHFD
	0040100A  |. |8BCC          MOV ECX,ESP
	0040100C  |? |55            PUSH EBP
	0040100D  |. |8BEC          MOV EBP,ESP
	0040100F  |? |51            PUSH ECX
	00401010  |? |E8 9E030000   CALL 004013B3
	00401015  |. |8BE5          MOV ESP,EBP
	00401017  |. |5D            POP EBP
	00401018  |. |9D            POPFD
	00401019  |. |58            POP EAX
	0040101A  |. |5B            POP EBX
	0040101B  |. |59            POP ECX
	0040101C  |. |5A            POP EDX
	0040101D  |. |5D            POP EBP
	0040101E  |. |5E            POP ESI
	0040101F  |? |5F            POP EDI
	00401020  |? |C3            RETN
	00401021  |? \E8 DCFFFFFF   CALL 00401002


	*/

		*p++ = 0xEB;*p++ = 0x1F; //        JMP SHORT 00401021
		*p++ = 0x57;//                   PUSH EDI
		*p++ = 0x56;//                   PUSH ESI
		*p++ = 0x55;//                   PUSH EBP
		*p++ = 0x52;//                   PUSH EDX
		*p++ = 0x51;//                   PUSH ECX
		*p++ = 0x53;//                   PUSH EBX
		*p++ = 0x50;//                   PUSH EAX
		*p++ = 0x9C;//                   PUSHFD

		*p++ = 0x8B;*p++ = 0xCC;//                MOV ECX,ESP
		*p++ = 0x55;//                   PUSH EBP
		*p++ = 0x8B;*p++ = 0xEC;//                 MOV EBP,ESP
		*p++ = 0x51;//                   PUSH ECX
		//*p++ = 0xE8;// 98030000          CALL 004013AB	//call filter function
		p = make_jmp_near( p,p,(const unsigned char *)filter_function,1 );
		*p++ = 0x8B;*p++ = 0xE5;//                 MOV ESP,EBP
		*p++ = 0x5D;//                   POP EBP

		*p++ = 0x9D ;//           POPFD
		*p++ = 0x58;//                   POP EAX
		*p++ = 0x5B;//                   POP EBX
		*p++ = 0x59;//                   POP ECX
		*p++ = 0x5A;//                   POP EDX
		*p++ = 0x5D;//                   POP EBP
		*p++ = 0x5E;//                   POP ESI
		*p++ = 0x5F;//                   POP EDI

		*p++ = 0xC3;//            RETN
		*p++ = 0xE8;*p++ = 0xDC;*p++ = 0xFF;*p++ = 0xFF;*p++ = 0xFF;//   CALL 00401002
		//orgin code end
		//*p++ = 0xE9;// 84030000          JMP 004013A7 //jmp to orgin code return

		ctx->orginal_code_size = hook_copy_code( orginal_code_ptr,100,p,sizeof(ctx->proxy_code) - (p-ctx->proxy_code),jmpcodelen );
		if( ctx->orginal_code_size < jmpcodelen )
		{
			memset( ctx,0,sizeof(struct _filter_context_) );
			return 0;
		}

		if( 0 == make_skip_array( ctx,p,sizeof(ctx->proxy_code) - (p-ctx->proxy_code),((const unsigned char*)orginal_code_ptr )+ctx->orginal_code_size,100 ) )
		{
			memset( ctx,0,sizeof(struct _filter_context_) );
			return 0;
		}

		memcpy( ctx->orginal_code,orginal_code_ptr,ctx->orginal_code_size );
		hook_make_executeable(ctx->proxy_code,sizeof(ctx->proxy_code));

	hook_write_memory( orginal_code_ptr,jmpcode,ctx->orginal_code_size );

	return 1;
}


int filter_install_x64( struct _filter_context_* ctx,const void* orginal_code_ptr,lpfn_filter_function filter_function,const void** bridgeslot_x64 )
{
	unsigned char* p;
	unsigned char	jmpcode[24] = {0};
	int				jmpcodelen = 0;

	memset( jmpcode,0x90,sizeof(jmpcode) );

	memset( ctx,0,sizeof(struct _filter_context_) );
	p = ctx->proxy_code;

	if( 0 == bridgeslot_x64 )
	{
		jmpcodelen = make_jmp_near( jmpcode,orginal_code_ptr,ctx->proxy_code,0 )-jmpcode;
		if( jmpcodelen<=0 )return 0;
	}else
	{
		(*(int*)bridgeslot_x64) ++;//最前面一个元素，存放当前数量,真正存放指针，从第2个元素开始
		bridgeslot_x64 += *(int*)bridgeslot_x64;
		*bridgeslot_x64  = ctx->proxy_code+sizeof(void*);
		//jmp qword[offset32]
		jmpcodelen = make_jmp_qword_disp( jmpcode,orginal_code_ptr,(const unsigned char*)bridgeslot_x64 )-jmpcode;
		if( jmpcodelen<=0 )return 0;

	}
	ctx->orginal_code_ptr = orginal_code_ptr;


	/*x64
	filter_function_pointer
	00 00 00 00 00 00 00 00
	0000000140001009 EB 43            jmp         000000014000104E 
	000000014000100B 41 57            push        r15  
	000000014000100D 41 56            push        r14  
	000000014000100F 41 55            push        r13  
	0000000140001011 41 54            push        r12  
	0000000140001013 41 53            push        r11  
	0000000140001015 41 52            push        r10  
	0000000140001017 41 51            push        r9   
	0000000140001019 41 50            push        r8   
	000000014000101B 57               push        rdi  
	000000014000101C 56               push        rsi  
	000000014000101D 55               push        rbp  
	000000014000101E 52               push        rdx  
	000000014000101F 51               push        rcx  
	0000000140001020 53               push        rbx  
	0000000140001021 50               push        rax  
	0000000140001022 9C               pushfq           
	0000000140001023 48 8B CC         mov         rcx,rsp 
	0000000140001026 55               push        rbp  
	0000000140001027 48 8B EC         mov         rbp,rsp 
	000000014000102A 51               push        rcx  
	000000014000102B FF 15 D0 FF FF FF call        qword ptr [140001001h] 
	0000000140001031 48 8B E5         mov         rsp,rbp 
	0000000140001034 5D               pop         rbp  
	0000000140001035 9D               popfq            
	0000000140001036 58               pop         rax  
	0000000140001037 5B               pop         rbx  
	0000000140001038 59               pop         rcx  
	0000000140001039 5A               pop         rdx  
	000000014000103A 5D               pop         rbp  
	000000014000103B 5E               pop         rsi  
	000000014000103C 5F               pop         rdi  
	000000014000103D 41 58            pop         r8   
	000000014000103F 41 59            pop         r9   
	0000000140001041 41 5A            pop         r10  
	0000000140001043 41 5B            pop         r11  
	0000000140001045 41 5C            pop         r12  
	0000000140001047 41 5D            pop         r13  
	0000000140001049 41 5E            pop         r14  
	000000014000104B 41 5F            pop         r15  
	000000014000104D C3               ret              
	000000014000104E E8 B8 FF FF FF   call        000000014000100B 

	//orgin code here
	//....
	//orgin code end
	FF 25 00 00 00 00 jmp         qword ptr [orgin_code_return_pointer] 

	orgin_code_return_pointer
	00 00 00 00 00 00 00 00
	*/

	//filter_function_pointer
	//	00 00 00 00 00 00 00 00
	*((void**)p) = (void*)filter_function;
	p+=sizeof(void*);

		*p++ = 0xEB;*p++ = 0x43;//            jmp         0000000140001050 
		*p++ = 0x41;*p++ = 0x57;//            push        r15  
		*p++ = 0x41 ;*p++ = 0x56;//            push        r14  
		*p++ = 0x41 ;*p++ = 0x55;//            push        r13  
		*p++ = 0x41 ;*p++ = 0x54;//            push        r12  
		*p++ = 0x41 ;*p++ = 0x53;//            push        r11  
		*p++ = 0x41 ;*p++ = 0x52;//            push        r10  
		*p++ = 0x41 ;*p++ = 0x51;//            push        r9   
		*p++ = 0x41 ;*p++ = 0x50;//            push        r8   
		*p++ = 0x57;//               push        rdi  
		*p++ = 0x56;//               push        rsi  
		*p++ = 0x55;//               push        rbp  
		*p++ = 0x52;//               push        rdx  
		*p++ = 0x51;//               push        rcx  
		*p++ = 0x53;//               push        rbx  
		*p++ = 0x50;//               push        rax 
		*p++ = 0x9C;//            pushfq

		*p++ = 0x48 ;*p++ = 0x8B ;*p++ = 0xCC;//         mov         rcx,rsp 
		*p++ = 0x55;//               push        rbp  
		*p++ = 0x48 ;*p++ = 0x8B ;*p++ = 0xEC;//         mov         rbp,rsp 
		*p++ = 0x51;//               push        rcx  
		*p++ = 0xFF ;*p++ = 0x15 ;*p++ = 0xD0 ;*p++ = 0xFF ;*p++ = 0xFF ;*p++ = 0xFF;// call        qword ptr [filter_function_pointer] 
		*p++ = 0x48 ;*p++ = 0x8B ;*p++ = 0xE5;//         mov         rsp,rbp 
		*p++ = 0x5D;//               pop         rbp 

		*p++ = 0x9D;				// popfq
		*p++ = 0x58;//               pop         rax  
		*p++ = 0x5B;//               pop         rbx  
		*p++ = 0x59;//               pop         rcx  
		*p++ = 0x5A;//               pop         rdx  
		*p++ = 0x5D;//               pop         rbp  
		*p++ = 0x5E;//               pop         rsi  
		*p++ = 0x5F;//               pop         rdi  
		*p++ = 0x41 ;*p++ = 0x58;//            pop         r8   
		*p++ = 0x41 ;*p++ = 0x59;//            pop         r9   
		*p++ = 0x41 ;*p++ = 0x5A;//            pop         r10  
		*p++ = 0x41 ;*p++ = 0x5B;//            pop         r11  
		*p++ = 0x41 ;*p++ = 0x5C;//            pop         r12  
		*p++ = 0x41 ;*p++ = 0x5D;//            pop         r13  
		*p++ = 0x41 ;*p++ = 0x5E;//            pop         r14  
		*p++ = 0x41 ;*p++ = 0x5F;//            pop         r15  
		*p++ = 0xC3; //              ret              
		*p++ = 0xE8 ;*p++ = 0xB8 ;*p++ = 0xFF ;*p++ = 0xFF ;*p++ = 0xFF;//   call        000000014000100B 

		//orgin code here
		//....
		//orgin code end
		//FF 25 00 00 00 00 jmp         qword ptr [orgin_code_return_pointer] 

	//orgin_code_return_pointer
		//00 00 00 00 00 00 00 00
		ctx->orginal_code_size = hook_copy_code( orginal_code_ptr,100,p,sizeof(ctx->proxy_code) - (p-ctx->proxy_code),jmpcodelen );
		if( ctx->orginal_code_size < jmpcodelen )
		{
			memset( ctx,0,sizeof(struct _filter_context_) );
			return 0;
		}

		if( 0 == make_skip_array( ctx,p,sizeof(ctx->proxy_code) - (p-ctx->proxy_code),((const unsigned char*)orginal_code_ptr )+ctx->orginal_code_size,100 ) )
		{
			memset( ctx,0,sizeof(struct _filter_context_) );
			return 0;
		}
	memcpy( ctx->orginal_code,orginal_code_ptr,ctx->orginal_code_size );
	hook_make_executeable(ctx->proxy_code,sizeof(ctx->proxy_code));

	hook_write_memory( orginal_code_ptr,jmpcode,ctx->orginal_code_size );

	return 1;
}

HOOKLIB_API	int filter_install( struct _filter_context_* ctx,const void* orginal_code_ptr,lpfn_filter_function filter_function,const void** bridgeslot_x64 )
{

	if( 0 == orginal_code_ptr || 0 == filter_function )return 0;

	if( IsX64 )
	{

		return filter_install_x64( ctx,orginal_code_ptr,filter_function,bridgeslot_x64 );
	}else
	{
		return filter_install_x86( ctx,orginal_code_ptr,filter_function );
	}
}

HOOKLIB_API	int filter_uninstall( struct _filter_context_* ctx )
{
	int n;
	if( 0 == ctx )return -1;
	if( 0 == ctx->orginal_code_ptr || ctx->orginal_code_size < 1)return -1;


	n = hook_write_memory( ctx->orginal_code_ptr,ctx->orginal_code,ctx->orginal_code_size );
	if( n >=0 )
	{
		memset( ctx,0,sizeof(struct _filter_context_) );
	}
	return n;
}

HOOKLIB_API int hook_is_readable( const void* memory,size_t size )
{
	return 0 == port_is_bad_read_ptr( (const void*)memory,size );
}

HOOKLIB_API int hook_is_writeable( const void* memory,size_t size )
{
	return 0 == port_is_bad_write_ptr( (const void*)memory,size);
}

struct  _filter_register_private
{
	struct  _filter_register_ reg;
	void*	rip ;//do not modify this value
};

HOOKLIB_API	void** filter_get_rsp( struct _filter_register_* _register_values_ )
{
	struct _filter_register_private* p = (struct _filter_register_private*)_register_values_;
	return (void**)(p+1);
}

HOOKLIB_API	int filter_skip_rip( struct _filter_context_* ctx,struct _filter_register_* _register_values_,int skip_count )
{

	struct _filter_register_private* p = (struct _filter_register_private*)_register_values_;

	if( skip_count <1 || skip_count >= MAX_SKIP_RIP )return 0;

	p->rip = ctx->skip_rip[skip_count];

	return 1;
}

