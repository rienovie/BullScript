global _start

    section .data
message: db "Hello world!", 10, 0

    section .text

fn_stackPush:
    pop rbx; pop return addr

    push rax
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9
    push r14
    push r11

    push rbx; push return addr

    ret

fn_stackPop:
    pop rbx; pop return addr

    pop r11
    pop r14
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi
    pop rax

    push rbx; push return addr

    ret

_func:
    mov r15, r14 ; input to temp
    call fn_stackPush
    mov r14, r15
    call r13
    mov r12, r11
    call fn_stackPop
    mov r11, r12

    ret

func_length:
    mov r11, r14

    l_length_nextChar:
    cmp byte [r11], 0
    je l_length_finish
    inc r11
    jmp l_length_nextChar

    l_length_finish:
    sub r11, r14

    ret

func_print:
    mov rax, 1
    mov rdi, 1
    mov rsi, r14

    mov r13, func_length
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
    mov r13, func_print
    call _func

    call _exit
