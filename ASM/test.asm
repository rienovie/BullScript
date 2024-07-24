global _start

    section .data
message: db "Hello great world!", 10, 0

    section .text

_end:
    mov rax, 60
    xor rdi, rdi
    syscall

_countChars:

    mov r8, r15

    l_nextChar:
    cmp byte [r8], 0
    je l_finsh
    inc r8
    jmp l_nextChar

    l_finsh:
    sub r8, r15
    ret

_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, message

    mov r15, message
    call _countChars
    mov rdx, r8

    syscall

    jmp _end
