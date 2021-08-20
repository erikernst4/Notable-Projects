extern BrokenExp_c
global BrokenExp_asm

extern malloc
extern free

section .rodata

a: DD 0,-4,4,8,4,-4,4,8,0,-4,4,8,-4,0,4,-4,-4,4,16,32,4,0,4,-4,-8,-16,0,8,0,4,-4,0,0,4,0,16,32,16,8,4

cero: times 4 DD 0

mascara1: times 4 DB 0x00, 0xFF, 0x00, 0x00

mascara2: times 4 DB 0xFF, 0x00, 0x00, 0x00


section .text
	BrokenExp_asm:
		push rbp
		mov rbp, rsp
		push rbx
		push r12
		push r13
		push r14
		push r15
		sub rsp, 8
		

	; Preservo parámetros de entrada	
		mov rbx, rsi 			; rbx = *dst
		mov r12, rdi			; r12 = *src
		xor r14, r14            ; Limpio
        xor r13, r13
        mov r13d, edx 			; r13d = width
		mov r14d, ecx			; r14d = height
		
	; Por cada fila hay 3 offsets. Los precalculo en una tabla de filas :
		mov edi, r14d
		call precalcularOffsets
		mov rsi, rax
		
		xor rcx, rcx			; índice filas

	; Para cada fila me guardo los 3 offsets
	.cicloFila:
		xor r11, r11			; índice columnas
		mov r15d, [rsi + 4]     ; r15d= offset jg
        mov edi, [rsi + 8]      ; edi = offset jb
		
	.cicloColumna:
		xor r8, r8  		    ; limpio
		xor r9, r9
		xor r10, r10

        mov r8d, [rsi]          ; r8d = offset jr lo calculo acá porque no quedan más registros de uso general       
        mov r9d, r15d
        mov r10d, edi

        add r8d, r11d           ; r8d = j + offset jr
        add r9d, r11d           ; r8d = j + offset jg
        add r10d, r11d          ; r8d = j + offset jb

    ; Hago el módulo width 
        cmp r8d, r13d                
        jge .restoJr
        cmp r8d, 0
        jge .modJg
        add r8d, r13d
        jmp .modJg    
    .restoJr:
        sub r8d, r13d
    .modJg:
        cmp r9d, r13d                
        jge .restoJg
        cmp r9d, 0
        jge .modJb
        add r9d, r13d
        jmp .modJb    
    .restoJg:
        sub r9d, r13d
    .modJb:
        cmp r10d, r13d                
        jge .restoJb
        cmp r10d, 0
        jge .continuo
        add r10d, r13d
        jmp .continuo    
    .restoJb:
        sub r10d, r13d
    
    .continuo:
	; Levanto los 4 pixeles desde [i][jr] = [r12 + fila * width + jr]
		mov rax, rcx 
		mul r13d	    		; eax = i * width
		add r8, rax	    		; edi = jr + i * width
		movdqu xmm1, [r12 + r8 * 4]	; xmmo = | src[i][jr + 3] | src[i][jr + 2] | src[i][jr + 1] | src[i][jr] | 
		
	; Levanto los 4 pixeles desde [i][jg]
		add r9, rax	    		; r9 = jg + i * width
		movdqu xmm2, [r12 + r9 * 4]	; xmm1 = | src[i][jg + 3] | src[i][jg + 2] | src[i][jg + 1] | src[i][jg] |

	; Levanto los 4 pixeles desde [i][jb]
		add r10, rax			; r10 = jb + fila * width
		movdqu xmm3, [r12 + r10 * 4]	; xmm2 = | src[i][jb + 3] | src[i][jb + 2] | src[i][jb + 1] | src[i][jb] |

	; Mergeo los bytes
		movdqu xmm0, [mascara1]
		pblendvb xmm1, xmm2		; me quedo con los .r de xmm1 y los .g de xmm2
		movdqu xmm0, [mascara2]
		pblendvb xmm1, xmm3		; me quedo con los .b de xmm3
		
	; Escribo en dst[i][j] = [rbx + rcx * width + rdx]
		add rax, r11			; eax = (i * width) + j
		movdqu [rbx + rax * 4], xmm1
		add r11d, 4
		cmp r11d, r13d
		jl .cicloColumna
        add rsi, 12		
        inc ecx
		cmp ecx, r14d
		jl .cicloFila
		
	; Libero la memoria de la tabla
        mov rax, 12
        mul r14
        sub rsi, rax
        mov rdi, rsi
        call free		

		add rsp, 8
		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
	ret
	precalcularOffsets:
		push rbp
		mov rbp, rsp
		push rbx
		push r12

		mov r12d, edi
	; pido espacio para 3 offsets (int32_t) por fila => 16 bytes * filas
		mov rax, 16 
		mul edi
		mov rdi, rax
		call malloc
		mov rbx, rax			    ; rbx puntero a la tabla
		mov rsi, rax

		xor rcx, rcx 			    ; índice filas
		xor rdx, rdx			    ; posición en el arreglo a
        mov r10, 40        

	.ciclo:
		xor r9, r9			        ; indice offsets
        mov edx, ecx		
        add edx, 10
		cmp edx, 40
		jl .escribo
		mov eax, edx
        xor rdx, rdx
        div r10d                    ; edx = (i + 10) % 40
		
	.escribo:
		mov r8d, [a + rdx * 4]	
		mov [rsi + r9 * 4], r8d 	; Escribo el primer offset
		inc r9b
		cmp r9b, 2			        ; repito la escritura 3 veces
		jg .finCiclo		
		add dl, 10
		cmp dl, 40
		jl .escribo
		sub dl, 40
		jmp .escribo

	.finCiclo:
		add rsi, 12	
		inc ecx
		cmp ecx, r12d
		jl .ciclo
		
		mov rax, rbx	
		
		pop r12
		pop rbx
		pop rbp
	ret
