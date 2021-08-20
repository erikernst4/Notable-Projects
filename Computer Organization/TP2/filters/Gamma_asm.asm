; extern Gamma_c
global Gamma_asm

; void Gamma_asm (uint8_t *src, uint8_t *dst, int width, int height, int src_row_size, int dst_row_size);
;                   rdi         rsi             rdx         rcx         r8                  r9

section .rodata
    div_val: times 4 dd 255
    BLANCO: times 16 db 255

section .text

; limpio registros

Gamma_asm:
    push rbp
    mov rbp, rsp
    push r12
    push r13
 ;   push r14
  ;  push r15
    
    ; pixel: 32 bits sin signo
    ; en memoria los pixeles se guardan en el orden B, G, R, A
    ; el ancho de la imagen es mayor a 16 pixeles y multiplo de 8
    
    mov r10d, edx
    xor rdx, rdx
    mov edx, r10d
    
    mov r10d, ecx
    xor rcx, rcx
    mov ecx, r10d

    mov r10d, r8d
    xor r8, r8
    mov r8d, r10d

    mov r10d, r9d
    xor r9, r9
    mov r9d, r10d

    mov r12d, edx
    shl r12, 2 ; r12d = width * 4

    xor r13, r13
    mov r13d, ecx ; r13d = height

    mov rcx, 0              ; rcx = indice fila
    mov r11, 0              ; r11 = indice columna
    .cicloHeight:
    cmp rcx, r13
    je .finCicloHeight
        .cicloWidht:
            cmp r11, r12
            je .finCicloWidht
            mov rax, rcx
            mul r8 ; rax = altura * row size
            add rax, r11
            lea r10, [rsi + rax]
;            movdqu xmm0, [BLANCO]
;            movdqu [r10], xmm0
            movdqu xmm0, [r10] 
            ; xmm0 = 4 pixeles
            ; xmm0 [ p4 | p3 | p2 | p1 ]
            
            ; xmm5 = [ 255 | 255 | 255 | 255 ] en single precision
            movdqu xmm5, [div_val]
            cvtdq2ps xmm5, xmm5

            ; xmm1 = p1
            pmovzxbd xmm1, xmm0 
            cvtdq2ps xmm1, xmm1
            ; xmm1 = [ a | r | g | b ]
            divps xmm1, xmm5
            ; xmm1 = [ a/255 | r/255 | g/255 | b/255 ]
            sqrtps xmm1, xmm1
            ; xmm1 = [ sqrt(a/255) | sqrt(r/255) | sqrt(g/255) | sqrt(b/255) ]
            mulps xmm1, xmm5
            ; xmm1 = [ sqrt(a/255)*255 | sqrt(r/255)*255 | sqrt(g/255)*255 | sqrt(b/255)*255 ]
            cvtps2dq xmm1, xmm1 ; convierto xmm1 a dwords
            psrldq xmm0, 4      ; shifteo xmm0 para que en los 4 bytes menos significativos se encuentre el próximo pixel            

            ; xmm2 = p2
            pmovzxbd xmm2, xmm0 
            cvtdq2ps xmm2, xmm2
            ; xmm2 = [ a | r | g | b ]
            divps xmm2, xmm5
            ; xmm2 = [ a/255 | r/255 | g/255 | b/255 ]
            sqrtps xmm2, xmm2
            ; xmm2 = [ sqrt(a/255) | sqrt(r/255) | sqrt(g/255) | sqrt(b/255) ]
            mulps xmm2, xmm5
            ; xmm2 = [ sqrt(a/255)*255 | sqrt(r/255)*255 | sqrt(g/255)*255 | sqrt(b/255)*255 ]
            cvtps2dq xmm2, xmm2 ; convierto xmm2 a dwords
            psrldq xmm0, 4      ; shifteo xmm0 para que en los 4 bytes menos significativos se encuentre el próximo pixel

            ; xmm3 = p3
            pmovzxbd xmm3, xmm0 
            cvtdq2ps xmm3, xmm3
            ; xmm3 = [ a | r | g | b ]
            divps xmm3, xmm5
            ; xmm3 = [ a/255 | r/255 | g/255 | b/255 ]
            sqrtps xmm3, xmm3
            ; xmm3 = [ sqrt(a/255) | sqrt(r/255) | sqrt(g/255) | sqrt(b/255) ]
            mulps xmm3, xmm5
            ; xmm3 = [ sqrt(a/255)*255 | sqrt(r/255)*255 | sqrt(g/255)*255 | sqrt(b/255)*255 ]
            cvtps2dq xmm3, xmm3 ; convierto xmm3 a dwords            
            psrldq xmm0, 4      ; shifteo xmm0 para que en los 4 bytes menos significativos se encuentre el próximo pixel

            ; xmm4 = p4
            pmovzxbd xmm4, xmm0 
            cvtdq2ps xmm4, xmm4
            ; xmm4 = [ a | r | g | b ]
            divps xmm4, xmm5
            ; xmm4 = [ a/255 | r/255 | g/255 | b/255 ]
            sqrtps xmm4, xmm4
            ; xmm4 = [ sqrt(a/255) | sqrt(r/255) | sqrt(g/255) | sqrt(b/255) ]
            mulps xmm4, xmm5
            ; xmm4 = [ sqrt(a/255)*255 | sqrt(r/255)*255 | sqrt(g/255)*255 | sqrt(b/255)*255 ]
            cvtps2dq xmm4, xmm4 ; convierto xmm4 a dwords            
            psrldq xmm0, 4      ; shifteo xmm0 para que en los 4 bytes menos significativos se encuentre el próximo pixel

            packssdw xmm1, xmm2 ; xmm2 = [ p2 | p1 ]
            packssdw xmm3, xmm4 ; xmm3 = [ p4 | p3 ] 
            packuswb xmm1, xmm3 ; xmm1 = [ p4 | p3 | p2 | p1 ]

            lea r10, [rsi + rax]
            movdqu [r10], xmm1

            add r11, 16          ; levanto de a 4 pixeles
            jmp .cicloWidht
        .finCicloWidht:
    mov r11, 0
    inc rcx
    jmp .cicloHeight    
    .finCicloHeight:
    pop r13
    pop r12
    pop rbp
ret