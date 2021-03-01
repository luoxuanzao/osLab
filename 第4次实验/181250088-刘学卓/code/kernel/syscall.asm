
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"


_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_print_str      equ  1;  打印字符串
_NR_sleep       equ 2  ;  不给进程分配时间片
_NR_P 				equ 3; P操作
_NR_V 				equ  4; V 操作

INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global print_str
global sleep
global p
global v



bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret


print_str:
	mov eax,_NR_print_str
	push ebx
	mov ebx, [esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

sleep:
	mov eax,_NR_sleep
	push ebx
	mov ebx,[esp+8]
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

p:
	mov eax, _NR_P
	push ebx
	mov ebx,[esp+8];
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret



v:
	mov eax, _NR_V
	push ebx
	mov ebx,[esp+8];
	int INT_VECTOR_SYS_CALL
	pop ebx
	ret

