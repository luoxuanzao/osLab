%include 'functions.asm'

SECTION .data
msg     db      'Please input x and y :',0Ah,0h
newLine   db  0Ah,0h


SECTION .bss
readChar:   resb 1
sign1: resb 1
sign2: resb 1
num1:   resb 21
num2:   resb 21

signMax: resb 1
maxNum: resb 21
minNum: resb 21
lenMin: resb 4
lenMax: resb 4
subPoint: resb 4
sub_result: resb 21


lenNum1: resb 4
lenNum2: resb 4
storeLenNum1: resb 4
storeLenNum2: resb 4
lenSum: resb 4
add_result: resb 22
add_temp: resb 1
carry: resb 1

mul_result: resb 42 

mul_temp: resb 1
pointNum1: resb 4
pointNum2: resb 4


result_point: resb 4

postVal:resb 1
preVal:resb 1

SECTION .text
global _start
_start:
    mov dword[num1], 0
    mov dword[num2], 0
    mov byte[lenNum1], 0
    mov byte[lenNum2], 0
    mov byte[sign1],1
    mov byte[sign2],1

;打印提示符
print:
    mov eax, msg
    call sprint

read_num1:      ;读取第一个数字

    call read_char  ;读取结果会存在readChar里
    cmp byte[readChar],45   ;如果读到负号就去标记num的正负
    je signnum1
    cmp byte[readChar], 32  ;判断读到空格就去读下一个数字
    je read_num2

    mov bl,byte[readChar]
    mov ecx, num1
    add ecx, dword[lenNum1] ;ecx是num1的指针
    mov byte[ecx], bl
    inc dword[lenNum1];lenNum1记录num1的长度
    jmp read_num1

signnum1:
    mov al,0
    mov byte[sign1],al 
    jmp read_num1

read_num2:    
    call read_char
    cmp byte[readChar],45
    je signnum2
    cmp byte[readChar], 0Ah
    je cal

    mov bl, byte[readChar]
    mov ecx, num2
    add ecx, dword[lenNum2]
    mov byte[ecx], bl
    inc dword[lenNum2]
    jmp read_num2
signnum2:
    mov eax,0
    mov al,0
    mov byte[sign2],al 
    jmp read_num2

cal:
    mov al,byte[sign1]
    mov ah,byte[sign2]
    add al,ah   
    cmp al,1  ;判断两个数的负号 如果都是正或负就做加法 否则做减法 al为1就代表做减法
    je sub
    jmp sum

sum:
    push dword[lenNum1]
    push dword[lenNum2]

    dec dword[lenNum1]
    dec dword[lenNum2]
    mov dword[lenSum], 0
    mov dword[carry], 0

sum_loop:


    
    ;ecx,edx,esi分别为num1,num2,sum的指针
    mov ecx, num1
    mov edx, num2
    mov esi, add_result
    add ecx, dword[lenNum1]
    add edx, dword[lenNum2]
    add esi, dword[lenSum]
    
    ;在bl中求和
    mov bl, byte[ecx]
    mov bh, byte[edx]
    sub bl, 48
    sub bh, 48
    add byte[carry], bh
    add byte[carry], bl

    cmp byte[carry],10;判断是否需要进位
    jl .not_carry

;进位carry设为1 否则设为0
.carry:
    mov bl, byte[carry]
    add bl, 48
    mov byte[add_temp], bl
    mov byte[carry], 1
    sub byte[add_temp], 10
    jmp .add

.not_carry:
    mov bl, byte[carry]
    add bl, 48
    mov byte[add_temp], bl
    mov byte[carry], 0

.add: 
    mov bl, byte[add_temp]
    mov byte[esi], bl
    
    inc dword[lenSum]
    dec dword[lenNum1]
    dec dword[lenNum2]
    
    ;判断有没有一个数加完了
    cmp dword[lenNum1],0
    jl num1_zero
    cmp dword[lenNum2],0
    jl num2_zero

    jmp sum_loop


num1_zero:
    cmp dword[lenNum2],0
    jl  deal_carry

    mov edx, num2
    mov ecx, dword[lenNum2]
    mov dword[lenNum1],ecx


    jmp continue

num2_zero:
    mov edx, num1

;统一用edx表示剩余的数的起始地址
continue:    
    
    mov ecx, edx
    mov esi, add_result
    add ecx, dword[lenNum1]
    add esi, dword[lenSum]
    
    ;在bl中求和
    mov bl, byte[ecx]
    sub bl, 48
    add byte[carry], bl

    cmp byte[carry],10
    jnl .carry1
    cmp byte[carry],10
    jl .not_carry1

.carry1:
    mov bl, byte[carry]
    add bl, 48
    mov byte[add_temp], bl
    mov byte[carry], 1
    sub byte[add_temp], 10

.not_carry1:
    mov bl, byte[carry]
    add bl, 48
    mov byte[add_temp], bl
    mov byte[carry], 0


    mov bl, byte[add_temp]
    mov byte[esi], bl
    
    inc dword[lenSum]
    dec dword[lenNum1]
    
    cmp dword[lenNum1],0
    jl deal_carry

    jmp continue

deal_carry:
    cmp byte[carry], 0
    je finish_sum
    mov esi, add_result
    add esi, dword[lenSum]

    mov byte[add_temp],1
    mov bl, byte[add_temp]
    mov byte[esi],bl

    inc dword[lenSum]

finish_sum:
    mov eax,0
    mov al,byte[sign1]
    add al,byte[sign2]
    cmp al,0
    je minus ;如果是零两个都是负数 要加负号
    jmp show_sumresult
minus:
    mov byte[readChar],45
    mov eax,readChar
    call cprint
show_sumresult:
    ;从后往前输出
    dec dword[lenSum]
    cmp dword[lenSum], 0
    jl mul
    mov esi, add_result
    add esi, dword[lenSum]

    mov eax, esi
    call cprint

    jmp show_sumresult

mul:
    
    mov eax,newLine
    call cprint
    ;恢复lenNum1和lenNum2
    pop eax
    mov dword[lenNum2],eax
    pop eax
    mov dword[lenNum1],eax


    dec dword[lenNum1]
    dec dword[lenNum2]
    mov dword[pointNum1],0
    mov ebx,0;
  
    ;外层num1，内层num2 num1是下面的数 num2每次要被乘
loop1:
    mov dword[pointNum2],0
    mov eax,dword[lenNum1]
    cmp dword[pointNum1], eax
    jg finish_mul

    loop2:
        mov eax, dword[lenNum2]
        cmp dword[pointNum2],eax
        jg out_loop

        ;计算两数相乘
        mov ecx, num1
        mov edx, num2
        mov esi, mul_result
        add ecx, dword[lenNum1]
        sub ecx, dword[pointNum1]
        add edx, dword[lenNum2]
        sub edx, dword[pointNum2]

        mov bl, byte[ecx]
        mov bh, byte[edx]
        
        sub bl, 48
        sub bh, 48


        mov eax,0
        mov al, bh
        mov bh,0
        mul ebx

        mov byte[mul_temp],al
        ;打印了mul_temp的结果是对的
        mov eax,0

        mov al,byte[mul_temp]
        ; call iprintLF
        
        ; 计算要加的位
    division:
        push edx
        mov edi,10
        idiv edi
        mov bh,al
        mov bl,dl
        pop edx
        ;检查除法结果正确
        ; call iprintLF
        ; mov al,dl
        ; call iprintLF

        mov eax,dword[pointNum1]
        add eax,dword[pointNum2]
        mov dword[result_point],eax
        add esi,dword[result_point]

        add byte[esi],bl
        add byte[esi+1],bh
        ; mov eax,0
        ; mov al,byte[esi]
        ; call iprintLF
        cmp byte[esi], 10
        jl mul_continue

    mul_carry:
        sub byte[esi],10
        ; mov eax,esi
        ; add eax,1
        add byte[esi+1],1

    mul_continue:
        inc dword[pointNum2]
        jmp loop2

out_loop:
    inc dword[pointNum1]
    jmp loop1


finish_mul:
    mov ecx,41
    mov esi,mul_result
    add esi,ecx

delete_zero:
  
    cmp byte[esi],0
    jne checksign
    dec ecx
    cmp ecx,0
    je checkZero
    mov esi,mul_result
    add esi,ecx
    jmp delete_zero

    mov edx,0
    mov ebx,0

checkZero:
    mov esi,mul_result
    add esi,ecx
    cmp byte[esi],0
    je show_result


checksign:
    mov eax,0
    mov al,byte[sign1]
    add al,byte[sign2]
    cmp al,1
    je mul_minus
    jmp show_result
mul_minus:
    mov byte[readChar],45
    mov eax,readChar
    call cprint
show_result:
    ; mov eax,msg
    ; call sprintLF

    cmp ecx,0
    jl finish_cal

    mov esi,mul_result
    add esi,ecx

    mov bl,byte[esi]
    add byte[esi],48
    mov dl,byte[esi]
    mov eax,esi
    call cprint
    dec ecx
    jmp show_result
    



sub:

    push dword[lenNum1]
    push dword[lenNum2]

    mov eax,dword[lenNum1]
    cmp eax,dword[lenNum2]
    jg MaxNum1

    mov eax,dword[lenNum1]
    cmp eax,dword[lenNum2]
    jl MaxNum2

    

    mov ecx,0
    mov esi, num1
    mov edi, num2
compare:
    cmp ecx,dword[lenNum1]
    je MaxNum1    

    mov esi, num1
    mov edi, num2

    add esi,ecx
    add edi,ecx

    mov al, byte[esi];al代表num1
    mov ah, byte[edi];ah代表num2
    cmp al,ah
    jl MaxNum2
    cmp al,ah
    jg MaxNum1
    inc ecx
    jmp compare
MaxNum1:

    mov eax,dword[lenNum1]
    mov dword[lenMax],eax
    mov eax,dword[lenNum2]
    mov dword[lenMin],eax

    mov al,byte[sign1]
    mov byte[signMax],al

    mov ebx,0

.maxloop1:
    cmp ebx, 21
    je subCal
    
    mov esi,maxNum
    mov edi,minNum
    mov ecx,num1
    mov edx,num2
    
    add esi,ebx
    add edi,ebx
    add ecx,ebx
    add edx,ebx

    mov al, byte[ecx]
    mov byte[esi],al
    mov ah, byte[edx]
    mov byte[edi],ah

    inc ebx
    jmp .maxloop1

MaxNum2:    


    mov eax,dword[lenNum1]
    mov dword[lenMin],eax
    mov eax,dword[lenNum2]
    mov dword[lenMax],eax
    ; mov eax,dword[lenMax]
    ; call iprintLF
    
    mov al,byte[sign2]
    mov byte[signMax],al

    mov ebx,0

.maxloop2:
    cmp ebx, 21
    je subCal
    
    mov esi,maxNum
    mov edi,minNum
    mov ecx,num2
    mov edx,num1
    
    add esi,ebx
    add edi,ebx
    add ecx,ebx
    add edx,ebx

    mov al, byte[ecx]
    mov byte[esi],al
    mov ah, byte[edx]
    mov byte[edi],ah

    inc ebx

    jmp .maxloop2

subCal:
    ;maxNum是较大的数，minNum2是较小的数
  
    ; mov eax,dword[lenMin]
    ; call iprintLF
    ; mov eax,dword[lenMax]
    ; call iprintLF

    ; mov eax,maxNum
    ; call sprintLF
    ; mov eax, minNum
    ; call sprintLF
    dec dword[lenMax]
    dec dword[lenMin]
    mov dword[subPoint],0
   

sub_loop:
    ;  mov eax,msg
    ; call sprintLF
    cmp dword[lenMin],0
    jl finish_sub

    mov esi,maxNum
    mov edi,minNum
    mov edx,sub_result

    add esi,dword[lenMax]
    add edi,dword[lenMin]
    add edx,dword[subPoint]

    mov al,byte[esi]
    mov bl,byte[edi]
    cmp al, bl
    jnl exSub 
    dec esi
    dec byte[esi]
    add al,10
exSub:
    sub al,bl
    add al,48
    ; mov byte[readChar],al
    ; mov eax,readChar
    ; call cprint
    ; mov eax,newLine
    ; call cprint
    mov byte[edx],al

    dec dword[lenMax]
    dec dword[lenMin]
    inc dword[subPoint]

    jmp sub_loop


    



    

finish_sub:

    ; mov eax,sub_result
    ; call cprint
restSub:
    cmp dword[lenMax],0
    jl  show_subresult

    mov esi, maxNum
    mov edi, sub_result
    add esi, dword[lenMax]
    add edi, dword[subPoint]

    mov al,byte[esi]
    cmp al,'0'
    jnl  next 
    
    add al,10
    dec esi 
    dec byte[esi]



next:
    mov byte[edi],al

    dec dword[lenMax]
    inc dword[subPoint]

    jmp restSub

show_subresult:
    dec dword[subPoint]
    mov ecx,dword[subPoint]
    mov esi, sub_result
delete_sub_zero:
    cmp ecx,0
    jl subfinial

    
    mov esi,sub_result
    add esi,ecx
    cmp byte[esi],48
    jne check_sub_sign
    dec ecx
    jmp delete_sub_zero


check_sub_sign:
    cmp byte[signMax],1
    je show_sub_loop
    mov byte[readChar],45
    mov eax, readChar
    call cprint

show_sub_loop:
    cmp ecx,0
    jl toMul
    
    mov esi, sub_result
    add esi,ecx

    mov al,byte[esi]
    mov byte[readChar],al
    mov eax,readChar
    call cprint
    dec ecx
    jmp show_sub_loop

subfinial:

    mov byte[readChar],48
    mov eax,readChar
    call cprint
toMul:
    jmp mul


finish_cal:
    mov eax,newLine
    call cprint
    call quit





