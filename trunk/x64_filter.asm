;Ê¾Àý´úÂë2.asm
;Óï·¨£ºML64
;ml64 x64_filter.asm /link /subsystem:windows /entry:Main user32.lib


.code
Main proc
int 3

label_filter	 dq 0

jmp label_get_jmp_address
label_proxy_entry:
;push rsp ;replace rip
push r15
push r14
push r13
push r12
push r11
push r10
push r9
push r8
push rdi
push rsi
push rbp
push rdx
push rcx
push rbx
push rax
pushfq

mov rcx,rsp

push rbp
mov rbp,rsp
push rcx
call qword ptr [label_filter]
mov rsp,rbp
pop rbp

popfq
pop rax
pop rbx
pop rcx
pop rdx
pop rbp
pop rsi
pop rdi
pop r8
pop r9
pop r10
pop r11
pop r12
pop r13
pop r14
pop r15
;pop rsp ;replace rip
ret

label_get_jmp_address:
call label_proxy_entry

;orginal code here
;...
;jmp to orginal code
jmp qword ptr [label_orgin_return]
label_orgin_return		 dq 0

ret

Main ENDP
end