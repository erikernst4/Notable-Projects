extern Broken_c
global Broken_asm

extern malloc
extern free

section .rodata
a: DD 0,-4,4,8,4,-4,4,8,0,-4,4,8,-4,0,4,-4,-4,4,16,32,4,0,4,-4,-8,-16,0,8,0,4,-4,0,0,4,0,16,32,16,8,4

cero: times 4 DD 0

mascara1: times 4 DB 0x00, 0xFF, 0x00, 0x00

mascara2: times 4 DB 0xFF, 0x00, 0x00, 0x00


section .text
	Broken_asm:
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
        mov r15, rax
		
	; Me hago un registro donde tengo el width 4 veces 
		movd xmm6, r13d 		; xmm6 = [ ?     | ?     | ?     | width ]
 		pslldq xmm6, 12		    ; xmm6 = [ width | 0     | 0     | 0     ]
 		movdqu xmm7, xmm6		; xmm7 = xmm6
		psrldq xmm6, 4			; xmm6 = [ 0     | width | 0     | 0     ]	
		por xmm6, xmm7			; xmm6 = [ width | width | 0     | 0     ]
		movdqu xmm7, xmm6		; xmm7 = xmm6
		psrldq xmm6, 8 		    ; xmm6 = [ 0     | 0     | width | width ]
		por xmm6, xmm7			; xmm6 = [ width | width | width | width ]
	

		xor rcx, rcx			; índice filas

	; Para cada fila me guardo los 3 offsets
	.cicloFila:
		xor r11, r11			; índice columnas
		movdqa xmm8, [rsi]		; xmm8 = [ 0 | offset jb | offset jg | offset jr]
		add rsi, 16
		
	.cicloColumna:
		xor r8, r8  		    ; limpio
		xor r9, r9
		xor r10, r10 
	; Pongo j en un registro xmm para sumarselo a los offsets	
		movd xmm4, r11d 		; xmm4 = [ ?     | ?     | ?     | j     ]
 		pslldq xmm4, 12		    ; xmm4 = [ j     | 0     | 0     | 0     ]
 		movdqu xmm5, xmm4		; xmm5 = xmm6
		psrldq xmm4, 4			; xmm4 = [ 0     | j     | 0     | 0     ]	
		por xmm4, xmm5			; xmm4 = [ j     | j     | 0     | 0     ]
		movdqu xmm5, xmm4		; xmm5 = xmm6
		psrldq xmm4, 8 		    ; xmm4 = [ 0     | 0     | j     | j     ]
		por xmm4, xmm5			; xmm4 = [ j     | j     | j     | j     ]
	
	; Sumo a los offsets con j y hago módulo width
		paddd xmm4, xmm8		; xmm4 = [ ?     | jb    | jg    | jr    ]
		movdqu xmm9, [cero]
		movdqu xmm10, xmm6
		pcmpgtd xmm10, xmm4
		pandn xmm10, xmm6		; Pongo width en los doubles que eran mayores a width
		psubd xmm4, xmm10       ; Resto width donde corresponda
		pcmpgtd xmm9, xmm4
		pand xmm9, xmm6		    ; Pongo width en los doubles que eran negativos
		paddd xmm4, xmm9        ; Sumo donde corresponda
		
	; Me guardo cada posición en un registro
		movd r8d, xmm4			; r8d = jr
		psrldq xmm4, 4			; xmm4 = [ 0     | ?     | jb    | jg    ]
		movd r9d, xmm4			; r9d = jg
		psrldq xmm4, 4			; xmm4 = [ 0     | 0     | ?     | jb    ]
		movd r10d, xmm4		    ; r10d = jg
		
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
		inc ecx
		cmp ecx, r14d
		jl .cicloFila
		
	; Libero la memoria de la tabla
        mov rdi, r15
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
		mov DWORD [rsi + r9 * 4], 0	; para mantener alineación a 16 bytes
		add rsi, 16	
		inc ecx
		cmp ecx, r12d
		jl .ciclo
		
		mov rax, rbx	
		
		pop r12
		pop rbx
		pop rbp
	ret
