
global _start

    section .bss
unintializedVariable: resd 64

    section .rodata
immutableMessage: db "Immute Hello!", 10, 0
aMessage: db "aValue is greater than anotherValue", 10, 0
%define bShouldPrint 1

    section .data
message: db "Hello world!", 10, 0
anotherMessage: dw "Hi", 10, 0
aValue: db 15
anotherValue: db 2

    section .text

fn_stackPush:
    pop r8; pop return addr

    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r14
    push r11

    push r8; push return addr

    ret

fn_stackPop:
    pop r8; pop return addr

    pop r11
    pop r14
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    push r8; push return addr

    ret

_func:
    mov r15, r14 ; input to temp
    call fn_stackPush ; pushes input/output
    mov r14, r15 ; move input back
    call r13 ; call func set
    mov r12, r11 ; output to temp
    call fn_stackPop
    mov r11, r12 ; move output back

    ret

fn_func_length:
    mov r11, r14

    l_length_nextChar:
    cmp byte [r11], 0
    je l_length_finish
    inc r11
    jmp l_length_nextChar

    l_length_finish:
    sub r11, r14

    ret

fn_func_print:
    mov rax, 1
    mov rdi, 1
    mov rsi, r14

    mov r13, fn_func_length
    call _func
    mov rdx, r11

    syscall

    ret

_exit:
    mov rax, 60
    mov rdi, 0
    syscall

_start:
    mov r14, message
    mov r13, fn_func_print
    call _func

    %if bShouldPrint
    mov r14, anotherMessage
    mov r13, fn_func_print
    call _func
    %endif

    ; TODO: this doesn't work right figure out why
    %if aValue < anotherValue
    mov r14, aMessage
    mov r13, fn_func_print
    call _func
    %endif

    call _exit
