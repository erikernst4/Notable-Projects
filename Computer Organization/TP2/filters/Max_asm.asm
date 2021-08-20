global Max_asm

section .rodata
    BLANCO: times 16 db 255

section .text
;  void Max_c(
;   uint8_t *src, rdi
;   uint8_t *dst, rsi
;   int width, edx
;   int height, ecx
;   int src_row_size, r8d
;   int dst_row_size) r9d
Max_asm:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    push rbx
    push r12
    push r13
    push r14
    push r15

    xor r12, r12
    mov r12d, edx ; r12d = width
    xor r13, r13
    mov r13d, ecx ; r13d = height
    xor r14, r14
    mov r14d, r8d ; r14d = src_row_size
    xor r15, r15
    mov r15d, r9d ; r15d = dst_row_size
    

    mov r8, r12
    shl r8, 2 ; r8 = width en bytes  = width * 4
    ; pinto de blanco todo dst para simplificar codigo, no es para nada lo mas optimo seguramente
    xor rbx, rbx ; rbx = byte columna
    xor rcx, rcx ; rcx = fila
    movdqu xmm0, [BLANCO] ; xmm0 = 0xFF x 16 = 4 pixeles
    .ciclo_height_blanco:
        cmp rcx, r13
        je .fin_ciclo_height_blanco
        ; voy haciendo de a 8 pixeles ya que es multiplo de 8 el ancho
        .ciclo_width_blanco:
            cmp rbx, r8
            je .fin_ciclo_width_blanco

            mov rax, rcx
            mul r15d
            add rax, rbx
            lea r10, [rsi + rax] ; r10 = rsi + rcx * r15d + rbx
            movdqu [r10], xmm0
            add r10, 16 ; avanzo 4 pixeles
            movdqu [r10], xmm0

            add rbx, 32
            jmp .ciclo_width_blanco
        .fin_ciclo_width_blanco:
        xor rbx, rbx
        inc rcx
        jmp .ciclo_height_blanco
    .fin_ciclo_height_blanco:

    sub r12, 3 ; r12 = width - 3
    sub r13, 3 ; r13 = height - 3
    xor rcx, rcx ; rcx = fila
    .ciclo_height:
        cmp rcx, r13
        jge .fin_ciclo_height
        xor rbx, rbx ; rbx = columna
        .ciclo_width:
            cmp rbx, r12
            jge .fin_ciclo_width

            ; creo matriz 4x4
            mov rax, r14
            mul rcx
            lea rax, [rax + rbx * 4] ; rax = r14d * rcx + rbx * 4
            lea r10, [rdi + rax] ; r10 = pos primer elemento matriz 4x4
            movdqu xmm0, [r10]
            movdqu xmm1, [r10 + r14]
            mov r11, r14
            shl r11, 1
            movdqu xmm2, [r10 + r11] ; r11d = r14d * 2
            add r11, r14
            movdqu xmm3, [r10 + r11] ; r11d = r14d * 3

            ; fila 1
            pmovzxbw xmm4, xmm0 ; xmm4 = b2|g2|r2|a2|b1|g1|r1|a1
            movdqu xmm5, xmm0
            psrldq xmm5, 8 ; xmm5 = ? | ? | bgra4 | bgra3
            pmovzxbw xmm5, xmm5 ; xmm5 = b4|g4|r4|a4|b3|g3|r3|a3
            phaddw xmm4, xmm5 ; xmm4 = b4+g4|r4+a4|b3+g3|r3+a3|b2+g2|r2+a2|b1+g1|r1+a1

            ; fila 2
            pmovzxbw xmm5, xmm1 ; xmm5 = b2|g2|r2|a2|b1|g1|r1|a1
            movdqu xmm6, xmm1
            psrldq xmm6, 8 ; xmm6 = ? | ? | bgra4 | bgra3
            pmovzxbw xmm6, xmm6 ; xmm6 = b4|g4|r4|a4|b3|g3|r3|a3
            phaddw xmm5, xmm6 ; xmm5 = b4+g4|r4+a4|b3+g3|r3+a3|b2+g2|r2+a2|b1+g1|r1+a1

            phaddw xmm4, xmm5 ; xmm4 = sum bgra 4 f2 | sum bgra 3 f2 | sum bgra 2 f2 | sum bgra 1 f2 | idem para f1
            
            ; fila 3
            pmovzxbw xmm5, xmm2
            movdqu xmm6, xmm2
            psrldq xmm6, 8
            pmovzxbw xmm6, xmm6
            phaddw xmm5, xmm6 

            ; fila 4
            pmovzxbw xmm6, xmm3
            movdqu xmm7, xmm3
            psrldq xmm7, 8
            pmovzxbw xmm7, xmm7
            phaddw xmm6, xmm7

            phaddw xmm5, xmm6 ; xmm5 = sum bgra 4 f4 | sum bgra 3 f4 | sum bgra 2 f4 | sum bgra 1 f4 | idem para f3

            ; empiezo a buscar el max de todos los acums de rgba
            movdqu xmm6, xmm4
            pmaxuw xmm6, xmm5 ; xmm6 = max(4f4, 4f2) | max(3f2, 3f4) | ...
            movdqu xmm7, xmm6
            psrldq xmm7, 8 ; xmm7 = ? | ? | ? | ? | max(4f4, 4f2) | max(3f2, 3f4) | ...
            pmaxuw xmm6, xmm7 ; xmm6 = ? | ? | ? | ? | max4 | max3 | max2 | max1
            movdqu xmm7, xmm6
            psrldq xmm7, 4 ; xmm7 = ? | ? | ? | ? | ? | ? | max4 | max3
            pmaxuw xmm6, xmm7 ; xmm6 = ? | ? | ? | ? | ? | ? | max24 | max13
            movdqu xmm7, xmm6
            psrldq xmm7, 2 ; xmm7 = ? | ? | ? | ? | ? | ? | ? | max24
            pmaxuw xmm6, xmm7 ; xmm6 = ? | ? | ? | ? | ? | ? | ? | maxtotal

            ; broadcast max
            pslldq xmm6, 14 ; xmm6 = max|0|0|...|0 
            movdqu xmm9, xmm6
            pshufhw xmm9, xmm9, 0xFF ; xmm9 = maxtotal|maxtotal|maxtotal|maxtotal|0|0|0|0
            psrldq xmm6, 14 ; xmm6 = 0|0|0|0|0|0|0|maxtotal
            pshuflw xmm6, xmm6, 0 ; xmm6 = 0 | 0 | 0 | 0 | maxtotal | maxtotal | maxtotal | maxtotal
            por xmm6, xmm9 ; xmm6 = maxtotal | maxtotal | maxtotal | maxtotal | maxtotal | maxtotal | maxtotal | maxtotal


            ; defino indice del pixel max en la matriz de 4x4 aplanada
            movdqu xmm7, xmm6
            pcmpeqw xmm7, xmm4 ; xmm7 = cmp max a acums filas 2 y 1
            movdqu xmm8, xmm6
            pcmpeqw xmm8, xmm5 ; xmm8 = cmp max a acums filas 4 y 3
            pabsw xmm8, xmm8
            pabsw xmm7, xmm7 ; calculo val abs de words de cada una para el pack
            packuswb xmm7, xmm8 ; xmm7 = 0 | 0 | ... | 1 en la pos del max | 0 | ...

            ; busco pos i y pos j del max
            xor r8, r8 ; r8 = indice columna
            xor r9, r9 ; r9 = indice fila
            .ciclo_buscar_pixel_max:
                cmp r8, 4
                jne .no_aumentar_fila
                xor r8, r8
                inc r9
                .no_aumentar_fila:
                pextrb r11d, xmm7, 0 ; r11b = byte menos significativo de la union de la mascara
                test r11b, 1
                jne .fin_ciclo_buscar_pixel_max
                psrldq xmm7, 1
                inc r8
                jmp .ciclo_buscar_pixel_max
            .fin_ciclo_buscar_pixel_max:
            
            mov rax, r14
            mul r9
            lea rax, [rax + r8 * 4] ; rax = r14d * r9d + r8d * 4
            movd xmm0, [r10 + rax] ; xmm0 = ? | ? | ? | pixel_max
            pslldq xmm0, 12 ; xmm0 = pixel_max | 0 | 0 | 0
            movdqu xmm1, xmm0
            psrldq xmm0, 12 ; xmm0 = 0 | 0 | 0 | pixel_max
            psrldq xmm1, 8 ; xmm1 = 0 | 0 | pixel_max | 0
            por xmm0, xmm1 ; xmm0 = 0 | 0 | pixel_max | pixel_max

            mov rax, r15
            mul rcx
            lea rax, [rax + rbx * 4] ; rax = r15 * rcx + rbx * 4
            lea r10, [rsi + rax] ; r10 = pos primer elemento matriz 4x4 en dst

            lea rax, [r10 + r15 + 4] ; rax = r10 + r14 + 4, offset a segunda fila, segunda columna
            movq [rax], xmm0
            add rax, r15 ; offset a tercera fila, segunda columna
            movq [rax], xmm0
            
            add rbx, 2
            jmp .ciclo_width
        .fin_ciclo_width:
        add rcx, 2
        jmp .ciclo_height
    .fin_ciclo_height:

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    add rsp, 8
    pop rbp
ret