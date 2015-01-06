;Ê¾Àý´úÂë2.asm
;Óï·¨£ºML64
;ml x32_filter.asm /link /subsystem:windows /entry:Main user32.lib

.386
.model FLAT 
.data

.code
Main proc
;int 3

jmp label_get_jmp_address
label_proxy_entry:

push edi
push esi
push ebp
push edx
push ecx
push ebx
push eax
pushfd

;save esp pointer
mov ecx,esp
push ebp
mov ebp,esp
;push esp pointer to argv0
push ecx
call label_filter_function
mov esp,ebp
pop ebp

popfd
pop eax
pop ebx
pop ecx
pop edx
pop ebp
pop esi
pop edi
ret

label_get_jmp_address:
call label_proxy_entry

;orginal code here
;...
jmp label_orgin_return
fill_offset db 900 dup(0)

label_orgin_return:
db 4 dup(0)
label_filter_function:
db 4 dup(0)
ret

Main ENDP
end