global _start

    section .data
message: db "Hello world!", 10, 0ah
fNum: db 3.567e20

    section .text

_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, message
    mov rdx, 13
    syscall

    mov rax, 1
    mov rdi, 1
    mov rsi, fNum ; need to look up conversion
    mov rdx, 5
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall
