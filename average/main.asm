section .data
    x dd 5, 3, 2, 6, 1, 7, 4
    y dd 0, 10, 1, 9, 2, 8, 5
    minus db '-'

section .bss
    buffer resb 12

section .text
    global _start

_start:
    ; Вычисление суммы разностей
    xor ecx, ecx
    xor ebx, ebx

loop:
    cmp ecx, 7
    je average
    
    mov eax, [x + ecx * 4]
    sub eax, [y + ecx * 4]
    add ebx, eax
    inc ecx
    jmp loop

average:
    ; Вычисление среднего
    mov eax, ebx
    cdq
    mov ecx, 7
    idiv ecx
    
    ; Сохраняем результат
    mov edi, eax
    
    ; Если отрицательное - выводим минус
    test eax, eax
    jns positive
    
    mov edx, 1
    mov ecx, minus
    mov ebx, 1
    mov eax, 4
    int 0x80
    
    mov eax, edi
    neg eax

positive:
    ; Конвертация в строку
    mov esi, buffer + 11
    mov byte [esi], 10
    dec esi

digit:
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [esi], dl
    dec esi
    test eax, eax
    jnz digit
    
    ; Вывод
    inc esi
    mov edx, buffer + 12
    sub edx, esi
    mov ecx, esi
    mov ebx, 1
    mov eax, 4
    int 0x80
    
    ; Выход
    mov eax, 1
    xor ebx, ebx
    int 0x80
