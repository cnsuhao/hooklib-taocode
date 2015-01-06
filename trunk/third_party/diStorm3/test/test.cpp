// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#define  SUPPORT_64BIT_OFFSET
#include "distorm.h"

void test()
{
	_CodeInfo info;
	_DInst out[100]={0};
	_DecodeResult res;
	_DecodedInst decodedInstructions;
	unsigned int decodedInstructionsCount = 0;

	info.code = (const uint8_t*)test;
	info.codeLen = 1024;
	info.codeOffset = (_OffsetType)test;
	info.features = DF_NONE;
	info.dt = Decode64Bits;

	res = distorm_decompose( &info,out,10,&decodedInstructionsCount );


	for( unsigned int i = 0;i<decodedInstructionsCount;++i)
	{
		distorm_format( &info,&out[i],&decodedInstructions );
		printf("%08I64x (%02d) %-24s %s%s%s\r\n", decodedInstructions.offset, decodedInstructions.size, (char*)decodedInstructions.instructionHex.p, (char*)decodedInstructions.mnemonic.p, decodedInstructions.operands.length != 0 ? " " : "", (char*)decodedInstructions.operands.p);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	test();


	_DecodeResult res;
	_DecodedInst decodedInstructions[1000];
	unsigned int decodedInstructionsCount = 0, i = 0;
	_OffsetType offset = (_OffsetType)test;
	unsigned int dver = distorm_version();
	printf("diStorm version: %d.%d.%d\n", (dver >> 16), ((dver) >> 8) & 0xff, dver & 0xff);

	res = distorm_decode( offset, (const unsigned char*)test,1024, Decode32Bits, decodedInstructions, 10, &decodedInstructionsCount);
	for (unsigned int i = 0; i < decodedInstructionsCount; i++) {
		printf("%08I64x (%02d) %-24s %s%s%s\r\n", decodedInstructions[i].offset, decodedInstructions[i].size, (char*)decodedInstructions[i].instructionHex.p, (char*)decodedInstructions[i].mnemonic.p, decodedInstructions[i].operands.length != 0 ? " " : "", (char*)decodedInstructions[i].operands.p);
	}



	return 0;
}

