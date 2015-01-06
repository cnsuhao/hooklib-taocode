15:13 2014/3/14 by choday

x86
32,64位hook库

提供inlinehook和importhook
有些时候，inlinehook会失败，请使用importhook

x64 系统上面，有些函数会失败,原因为在遇到jmp qword[offset32],mov rax,qword[offset32] 这一类带有内存地址的，没有处理

跳转方法jmp qword[offset32] 6字节
如果空间足够放后面的[offset32],那么一共14字节
如果不够放，则申请新的内存来放跳转地址[offset32]
