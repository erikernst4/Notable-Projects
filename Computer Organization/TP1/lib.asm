global intCmp
global intClone
global intDelete
global intPrint
global strCmp
global strClone
global strDelete
global strPrint
global strLen
global arrayNew
global arrayGetSize
global arrayAddLast
global arrayGet
global arrayRemove
global arraySwap
global arrayDelete
global listNew
global listGetSize
global listAddFirst
global listGet
global listRemove
global listSwap
global listClone
global listDelete
global listPrint
global cardNew
global cardGetSuit
global cardGetNumber
global cardGetStacked
global cardCmp
global cardClone
global cardAddStacked
global cardDelete
global cardPrint

extern malloc
extern free
extern fprintf

extern arrayPrint
extern listAddLast
extern listPrint

extern getCompareFunction
extern getCloneFunction
extern getDeleteFunction
extern getPrintFunction



section .rodata:
    format_int: DB '%d', 0
    format_string: DB '%s', 0
    null: DB 'NULL', 0
    llaveizq: DB '{', 0
    llaveder: DB '}', 0
    guion: DB '-', 0


section .text
    intCmp: 
    ; puntero a en rdi
    ; puntero b en rsi

	    mov edi, [rdi]
	    cmp edi, [rsi]
	    je .igualcero
	    jg .mayor
	    jl .menor

        .igualcero:  
	        mov eax, 0
	        ret

        .mayor:
	        mov eax, -1
	        ret

        .menor:
	        mov eax, 1
	        ret
       
        intClone:
        ; puntero a en rdi
            push rbp
            mov rbp, rsp
            push rbx
            sub rsp, 8

            mov ebx, [rdi]
            mov rdi, 4
            xor rax, rax            
            call malloc
            mov [rax], ebx

            add rsp, 8
            pop rbx
            pop rbp
        ret
        
        intDelete:
            jmp free
        ret

        intPrint:
        ; puntero a en rdi, puntero pFile en rsi
            push rbp
            mov rbp, rsp
            push rbx
            sub rsp, 8

            mov ebx, [rdi]
            mov rdi, rsi
            mov rsi, format_int
            xor rax, rax           ; Convención C: en RAX van cuántos registros XMM se usan
            mov edx, ebx
            call fprintf

            add rsp, 8
            pop rbx
            pop rbp
        ret

        ; ** String **

        ; int32_t strCmp(char* a, char* b)
        strCmp:
        ; puntero a a en rdi, puntero a b en rsi
            push rbp
            mov rbp, rsp
            push rbx
            push r12

            .loop:
            mov bl, [rdi]
            mov r12b, [rsi]
            cmp bl, 0
            je .a_es_cero
            cmp r12b, 0
            je .mayor
            cmp bl, r12b
            jg .mayor
            jl .menor
            add rdi, 1
            add rsi, 1
            jmp .loop

            .a_es_cero:
            cmp r12b, 0
            je .igual
            jmp .menor

            .mayor:
            mov eax, -1
            jmp .fin

            .menor:
            mov eax, 1
            jmp .fin

            .igual:
            mov eax, 0
            jmp .fin
            
            .fin:
            pop r12
            pop rbx
            pop rbp
            ret

        ; char* strClone(char* a)
        strClone:       
        ; puntero a el string en rdi
            push rbp
            mov rbp, rsp

        ; bl para mover los chars, r12 para las pos. de memoria originales, r13 para las copia
            push rbx
            push r12
            push r13
            sub rsp, 8
            mov r12, rdi

        ; calcula el tamaño y se lo pide a malloc
            call strLen
            mov rdi, rax
            inc rdi
            call malloc

        ; mueve la primer pos a r13
            mov r13, rax

            .loop:
            mov bl, [r12]
            mov [r13], bl
            add r13, 1
            add r12, 1
            cmp bl, 0
            je .fin
            jmp .loop

            .fin:
            add rsp, 8
            pop r13
            pop r12
            pop rbx
            pop rbp            
        ret

        ; void strDelete(char* a)
        strDelete:
            jmp free
        ret

        ; void strPrint(char* a, FILE* pFile)
        strPrint:
        ; puntero a en rdi, puntero pfile en rsi
            push rbp
            mov rbp, rsp
            push rbx
            sub	rsp, 8

            mov bl, [rdi]
            cmp bl, 0 		; if a == null
            je .empty
            
            mov rdx, rdi	; else rdx = a
            jmp .print

            .empty:			; then rdx = &'NULL'
            mov rdx, null            

            .print:			; print *rdx
            mov rdi, rsi
            mov rsi, format_string
            xor rax, rax       
            call fprintf

            add rsp, 8
            pop rbx
            pop rbp
           ret

        ; uint32_t strLen(char* a)
        strLen:
        ; puntero a el string en rdi
            push rbp
            mov rbp, rsp

        ; bl se va a usar para comparar los chars y eax cuenta la longitud
            push rbx
            sub rsp, 8
            mov eax, 0

        .loop:
            mov bl, [rdi]
            cmp bl, 0
            je .fin
            add rdi, 1
            add eax, 1
            jmp .loop

        .fin:
        	add rsp, 8
            pop rbx
            pop rbp
            ret

        ; ** Array **

        ; array_t* arrayNew(type_t t, uint8_t capacity)
        arrayNew:
            push rbp
            mov rbp, rsp
            push rbx
            push r12
            push r13
            sub rsp, 8            

            mov r13, rsi
            mov rbx, rdi                ; preservo los parámetros de entrada
        ; pido espacio para el arreglo                                    
            mov rax, 8
            mul rsi                    
            mov rdi, rax                
            xor rax, rax    
            mov r12, rdi            
            call malloc            
            mov rdi, rax
            add r12, rax          

        .inicializarVector:             ; pongo en null al arreglo de punteros
            mov QWORD [rdi], 0         
            add rdi, 8
            cmp r12, rdi                 
            jne .inicializarVector

            mov r12, rax                ; rcx tiene el puntero al arreglo
            ; pido memoria para el struct array que necesita 16 bytes        
            mov rdi, 16
            xor rax, rax                    
            call malloc
            mov [rax], ebx              ; type (int)
            mov BYTE [rax + 4], 0       ; size   
            mov [rax + 5], r13b         ; capacity                
            mov [rax + 8], r12          ; data
                
            add rsp, 8
            pop r13
            pop r12            
            pop rbx
            pop rbp
        ret

        ; uint8_t  arrayGetSize(array_t* a)
        arrayGetSize:
            xor rax, rax                            ; limpio rax   
            mov al, [rdi + 4]        
        ret

        ; void  arrayAddLast(array_t* a, void* data)
        arrayAddLast:
        
            push rbp
            mov rbp, rsp
            push rbx
            push r12
            push r13
            sub rsp, 8
            
            mov r12, rdi
            mov r13, rsi
        ; Primero se chequea si se puede agregar (si size < capacity)
            xor rbx, rbx                            ; limpio rbx      
            mov bl, [rdi + 4]
            cmp bl, [rdi + 5]
            je .fin

            ; Hago una copia del dato
            xor rdi, rdi            
            mov edi, [r12]              ; edi = array.type  
            call getCloneFunction
            mov rdi, r13                ; rdi => puntero al data
            call rax                    ; ahora rax tiene el puntero al dato copia

            ; aumento el size y después escribo
            inc bl
            mov [r12 + 4], bl            
            mov r12, [r12 + 8]                      ; r12 = data
            mov [r12 + rbx * 8 - 8], rax            ; escritura
            
        .fin:
            add rsp, 8
            pop r13            
            pop r12
            pop rbx
            pop rbp 
 
        ret

        ; void* arrayGet(array_t* a, uint8_t i)
        arrayGet:
            
            mov al, [rdi + 4]
            cmp al, 0
            je .fueraDeRango
            dec al             
            cmp sil, al
            ja .fueraDeRango            ; i <= size - 1                

            mov rax, [rdi + 8]          ; rax = array.data
            mov rax, [rax + rsi * 8]                    
            ret
        
        .fueraDeRango:
            xor rax, rax
            ret
        

        ; void* arrayRemove(array_t* a, uint8_t i)
        arrayRemove:
            push rbp
            mov rbp, rsp
            push rbx
            push r12           

            xor rbx, rbx            
            mov bl, [rdi + 4]           ; bl -> arreglo.size
            cmp bl, 0
            je .fueraDeRango
            dec bl             
            cmp sil, bl                 ; i <= size - 1
            ja .fueraDeRango
        
            ; escribo el size decrementado y reacomodo el arreglo de punteros si es necesario
            
            mov [rdi + 4], bl            
            mov rdi, [rdi + 8]                     
            mov rax, [rdi + rsi * 8]        ; rax queda con el puntero a retornar
            mov QWORD [rdi + rsi * 8], 0            
                        
            cmp bl, sil            
            je .fin                         ; no reacomodo si borré el último elemento              
                                
        .reacomodo:            
            mov r12, [rdi + rsi * 8 + 8]             
            mov [rdi + rsi * 8], r12
            inc sil
            cmp sil, bl             
            jne .reacomodo
            mov QWORD [rdi + rsi * 8], 0            
            jmp .fin            

        .fueraDeRango:
            xor rax, rax
            jmp .fin
        
        .fin:    
            pop r12
            pop rbx
            pop rbp

        ret

        ; void  arraySwap(array_t* a, uint8_t i, uint8_t j)
        arraySwap:
            push rbp
            mov rbp, rsp
            push rbx
            push r12 

            mov al, [rdi + 4]
            cmp al, 0
            je .fin
            dec al             
            cmp sil, al
            ja .fin            ; i fuera de rango  
            cmp dl, al
            ja .fin            ; j fuera de rango
            
            mov rax, [rdi + 8]        
        ; me guardo cada puntero y los swapeo           
            mov rbx, [rax + rsi * 8]
            mov r12, [rax + rdx * 8]
            mov [rax + rsi * 8], r12            
            mov [rax + rdx * 8], rbx 

        .fin:
            pop r12
            pop rbx
            pop rbp        
        ret

        ; void  arrayDelete(array_t* a)
        arrayDelete:
            push rbp
            mov rbp, rsp
            push rbx
            push r12
            push r13
            push r14
 
        ; Almaceno en r14 la funcDelete de los datos del arreglo          
            mov r12, rdi        
            mov edi, [r12]
            call getDeleteFunction
            mov r14, rax

        ; 1) borro todos los datos del array
                
            mov bl, [r12 + 4]
            cmp bl, 0
            je .paso2           
            mov rbx, [r12 + 8]
            xor r13, r13

        .ciclo:            
            mov rdi, [rbx + r13 * 8]
            call r14
            inc r13
            cmp r13b, [r12 + 4]
            jl .ciclo

        ; 2) borro el arreglo de punteros
        .paso2:
            mov rdi, [r12 + 8]
            call free

        ; 3) borro la estructura
            mov rdi, r12
            call free

            pop r14
            pop r13
            pop r12
            pop rbx
            pop rbp 
        ret

        ; ** Lista **

        ; list_t* listNew(type_t t)
        listNew:
        push rbp
        mov rbp, rsp
        push rbx
        sub rsp, 8

        ;me guardo t
        mov ebx, edi

        ;pido memoria para el struct
        mov rdi, 24
        xor rax, rax
        call malloc

        ;pego los elems
        mov [rax], ebx
        mov BYTE [rax + 4], 0
        mov QWORD[rax + 8], 0
        mov QWORD [rax + 16], 0 

        add rsp, 8
        pop rbx
        pop rbp
        ret

        ; uint8_t  listGetSize(list_t* l)
        listGetSize:
        xor rax, rax
        mov al, [rdi + 4] 
        ret

        ; void listAddFirst(list_t* l, void* data)
        listAddFirst:
    ; a lo largo de esta funcion rbx = &lista, r13 = &nodo, r12 es auxiliar
        push rbp
        mov rbp, rsp
        push rbx
        push r12
        push r13
        sub rsp, 8

    ;nos guardamos el input
        mov rbx, rdi
        mov r12, rsi

    ;pedimos memoria para el nodo
        mov rdi, 24
        call malloc
        mov r13, rax

    ;escribimos en el struct del nodo
    ;clonamos data
      	  mov edi, [rbx]
      	  call getCloneFunction
      	  mov rdi, r12 					; ya no necesito r12 por eso lo sobreescribo despues
      	  call rax

        mov [r13], rax 					; nodo.data = data
        mov QWORD [r13 + 16], 0			; nodo.anterior = null
        mov r12, [rbx + 8]
        mov [r13 + 8], r12  			; nodo.siguiente = lista.primero

      ;if lista esta vacia
        cmp BYTE [rbx + 4], 0
        je .isempty

      ;else
      	mov r12, [rbx + 8]
      	mov [r12 + 16], r13 			;lista.primero.anterior = &nodo 
      	jmp .fin

      ;then
      	.isempty:
      	mov [rbx + 16], r13 			; lista.ultimo = &nodo

      	.fin:
      	mov [rbx + 8], r13				; lista.primero = &nodo
      	mov r12b, [rbx + 4]
      	inc r12b
      	mov [rbx + 4], r12b				; lista.size += 1

        add rsp, 8
        pop r13
        pop r12
        pop rbx
        pop rbp
        ret

        ; void* listGet(list_t* l, uint8_t i)
        listGet:
        push rbp
        mov rbp, rsp
        push rbx
        push r12

        mov bl, sil
        cmp bl, [rdi + 4]
        jae .outofrange
        mov r12, [rdi + 8] 

        .loop:
        cmp bl, 0
        je .fin
        mov r12, [r12 + 8]
        sub bl, 1
        jmp .loop

        .fin:
        mov rax, [r12]
        jmp .restauroPila

        .outofrange:
        mov rax, 0

        .restauroPila:
        pop r12
        pop rbx
        pop rbp
        ret

        ; void* listRemove(list_t* l, uint8_t i)
        listRemove:
        push rbp
        mov rbp, rsp
        push rbx
        push r12

        mov bl, sil									;	
        cmp bl, [rdi + 4]							; chequear que la posicion exista
        jae .outofrange								;
        mov r12, [rdi + 8]

        .loop:										; iteramos hasta que r12 apunta al nodo que queremos
        cmp bl, 0
        je .endofloop
        mov r12, [r12 + 8]
        sub bl, 1
        jmp .loop

        .endofloop:
        mov rcx, [r12 + 8]							; rcx = nodo.siguiente 
        mov rdx, [r12 + 16]							; rdx = nodo.anterior
		cmp rcx, 0
        je .islast									
        cmp rdx, 0
        je .isfirst
        mov [rdx + 8], rcx 							; nodo.anterior.siguiente = nodo.siguiente
        mov [rcx + 16], rdx 						; nodo.siguiente.anterior = nodo.anterior
        jmp .fin
      
        .islast:
        cmp rdx, 0
        je .islastandfirst
        mov [rdi + 16], rdx 						; lista.ultimo = nodo.anterior
        mov [rdx + 8], rcx  						; nodo.anterior.siguiente = null
        jmp .fin

        .isfirst:
        mov [rdi + 8], rcx							; lista.primero = nodo.siguiente
        mov QWORD [rcx + 16], 0						; nodo.siguiente.anterior = null
        jmp .fin

        .islastandfirst:
        mov QWORD [rdi + 8], 0						; lista.primero = null
        mov QWORD [rdi + 16], 0 					; lista.ultimo = null
        jmp .fin


        .fin:
        mov bl, [rdi + 4]
        dec bl
        mov [rdi + 4], bl
        mov rbx, [r12]
        mov rdi, r12
        call free
        mov rax, rbx

        pop r12
        pop rbx
        pop rbp
        ret

        .outofrange:
        mov rax, 0
        pop r12
        pop rbx
        pop rbp
        ret

        ; void  listSwap(list_t* l, uint8_t i, uint8_t j)
        listSwap:
        mov r8b, [rdi + 4]                          ; chequeamos que ambos indices esten bien
        cmp sil, r8b
        jae .outofrange 
        cmp dl, r8b
        jae .outofrange
        mov r8, [rdi + 8]

        cmp r8b, 1                                  ; si la longitud es 1 ni me gasto
        je .outofrange
        cmp sil, dl                                 ; same si son el mismo
        je .outofrange

        .loop1:                                     ; loopeamos hasta que r8 = i
        cmp sil, 0
        je .endofloop1
        mov r8, [r8 + 8]
        sub sil, 1
        jmp .loop1 

        .endofloop1:
        mov r9, [rdi + 8]
        .loop2:                                     ; loopeamos hasta que  r9 = j
        cmp dl, 0
        je .endofloop2
        mov r9, [r9 + 8]
        sub dl, 1
        jmp .loop2

        .endofloop2:

        mov rcx, [r8 + 8]                           ; rcx = i.siguiente
        mov rdx, [r8 + 16]                          ; rdx = i.anterior
        mov r10, [r9 + 8]                           ; r10 = j.siguiente
        mov r11, [r9 + 16]                          ; r11 = j.anterior

        cmp rcx, 0                                  ; empiezan los cases para el swap de punteros
        je .i_last
        cmp rdx, 0
        je .i_first
        cmp r10, 0
        je .i_normal_j_last
        cmp r11, 0
        je .i_normal_j_first
        jmp .i_normal_j_normal
        
        .i_last:                                    ; si i es ultimo hay que chequar j
        cmp r11, 0
        je .i_last_j_first
        jmp .i_last_j_normal

        .i_first:                                   ; si i es primero hay que chequear j 
        cmp r10, 0
        je .i_first_j_last
        jmp .i_first_j_normal

                                                    ; y a partir de aca estan hardcodeados los casos
        .i_normal_j_normal:
        mov [rcx + 16], r9                          ; i.siguiente.anterior = j
        mov [rdx + 8], r9                           ; i.anterior.siguiente = j
        mov [r10 + 16], r8                          ; j.siguiente.anterior = i                          
        mov [r11 + 8], r8                           ; j.anterior.siguiente = i
        jmp .fin 

        .i_first_j_normal:
        mov [rcx + 16], r9                          ; i.siguiente.anterior = j
        mov [rdi + 8], r9                           ; lista.primero = j
        mov [r10 + 16], r8                          ; j.siguiente.anterior = i
        mov [r11 + 8], r8                           ; j.anterior.siguiente = i
        jmp .fin

        .i_last_j_normal:
        mov [rdi + 16], r9                          ; lista.ultimo = j
        mov [rdx + 8], r9                           ; i.anterior.siguiente = j
        mov [r10 + 16], r8                          ; j.siguiente.anterior = i
        mov [r11 + 8], r8                           ; j.anterior.siguiente = i
        jmp .fin

        .i_normal_j_last:
        mov [rcx + 16], r9                          ; i.siguiente.anterior = j
        mov [rdx + 8], r9                           ; i.anterior.siguiente = j
        mov [rdi + 16], r8                          ; lista.ultimo = i
        mov [r11 + 8], r8                           ; j.anterior.siguiente = i
        jmp .fin

        .i_normal_j_first:
        mov [rcx + 16], r9                          ; i.siguiente.anterior = j
        mov [rdx + 8], r9                           ; i.anterior.siguiente = j
        mov [r10 + 16], r8                          ; j.siguiente.anterior = i
        mov [rdi + 8], r8                           ; lista.primero = i
        jmp .fin

        .i_last_j_first:
        mov [rdi + 16], r9                          ; lista.ultimo = j
        mov [rdx + 8], r9                           ; i.anterior.siguiente= j
        mov [r10 + 16], r8                          ; j.siguiente.anterior = j
        mov [rdi + 8], r8                           ; lista.primero = i
        jmp .fin

        .i_first_j_last:
        mov [rcx + 16], r9                          ; i.siguiente.anterior = j
        mov [rdi + 8], r9                           ; lista.primero = j
        mov [rdi + 16], r8                          ; lista.ultimo = i
        mov [r11 + 8], r8                           ; j.anterior.siguiente = i

        .fin:
        cmp rcx, r9
        je .j_nextoi                                ; si se cumple alguna de estas dos el swap clasico se rompe y quedan apuntandose a si mismos
        cmp r10, r8
        je .i_nextoj

        mov [r9 + 8], rcx                           ; j.siguiente = i.siguiente
        mov [r9 + 16], rdx                          ; j.anterior = i.anterior
        mov [r8 + 8], r10                           ; i.siguiente = j.siguiente
        mov [r8 + 16], r11                          ; i.anterior = j.anterior
        ret

        .j_nextoi:
        mov [r9 + 8], r8                            ; j.siguiente = i
        mov [r9 + 16], rdx                          ; j.anterior = i.anterior
        mov [r8 + 8], r10                           ; i.siguiente = j.siguiente
        mov [r8 + 16], r9                           ; i.anterior = j
        ret

        .i_nextoj:
        mov [r9 + 8], rcx                           ; j.siguiente = i.siguiente
        mov [r9 + 16], r8                           ; j.anterior = i
        mov [r8 + 8], r9                            ; i.siguiente = j
        mov [r8 + 16], r11                          ; i.anterior = j.anterior
        ret

        .outofrange:
        ret
        
        listClone:
            push rbp
            mov rbp, rsp
            push rbx
            push r12
            push r13
            sub rsp, 8

            mov rbx, rdi							; preservo los argumentos de entrada
            xor rdi, rdi            
            mov edi, [rbx]
            call listNew
            mov r13, rax            
            mov r12, [rbx + 16]
            
        .ciclo:
            cmp r12, 0
            je .fin
            mov rsi, [r12]
            mov rdi, r13            
            call listAddFirst
            mov r12, [r12 + 16]
            jmp .ciclo
            
        .fin:
            mov rax, r13            

            add rsp, 8
            pop r13            
            pop r12
            pop rbx
            pop rbp
    
        ret

        ; void listDelete(list_t* l)
        listDelete:
        push rbp
        mov rbp, rsp
        push rbx
        push r12
        push r13
        push r14

        ; me guardo la entrada
        mov r12, rdi

        ; nos guardamos la funcion delete
        mov edi, [rdi]
        call getDeleteFunction
        mov rbx, rax
		mov r13, [r12 + 8]

        .loop:
        cmp r13, 0
        je .endofloop
        mov rdi, [r13]
        call rbx
        mov r14, [r13 + 8]
        mov rdi, r13
        call free
        mov r13, r14
        jmp .loop

        .endofloop:
        mov rdi, r12
        call free

        pop r14
        pop r13
        pop r12
        pop rbx
        pop rbp
        ret

        ; ** Card **

        ; card_t* cardNew(char* suit, int32_t* number)
        cardNew:
            push rbp
            mov rbp, rsp
            push rbx
            push r12
            push r13
            sub rsp, 8

        ; preservo los parámetros de entrada
            mov rbx, rdi
            mov r12, rsi 
    
        ; pido memoria para la estructura
            mov rdi, 24
            call malloc
            mov r13, rax

        ; creo copia del suit y escribo en la estuctura
            mov rdi, rbx
            call strClone
            mov [r13], rax
        
        ; creo copia del entero y escribo
            mov rdi, r12
            call intClone
            mov [r13 + 8], rax

        ; creo una lista y escribo su puntero
            mov rdi, 3
            call listNew
            mov [r13 + 16], rax
        
            mov rax, r13            

            add rsp, 8
            pop r13
            pop r12
            pop rbx
            pop rbp
        ret

        ; char* cardGetSuit(card_t* c)
        cardGetSuit:
            mov rax, [rdi]
        ret

        ; int32_t* cardGetNumber(card_t* c) 
        cardGetNumber:
            mov rax, [rdi + 8]
        ret

        ; list_t* cardGetStacked(card_t* c)
        cardGetStacked:
            mov rax, [rdi + 16]
        ret

        ; int32_t cardCmp(card_t* a, card_t* b)
        cardCmp:
            push rbp
            mov rbp, rsp
            push rbx
            push r12

            mov rbx, rdi
            mov r12, rsi

            ; comparo suits
            mov rdi, [rbx]
            mov rsi, [r12]
            call strCmp
            cmp rax, 0
            jne .fin
            
            ; si los suits son iguales comparo los number
            mov rdi, [rbx + 8]
            mov rsi, [r12 + 8]            
            call intCmp
            
        .fin:
            pop r12
            pop rbx
            pop rbp
        ret

        ; card_t* cardClone(card_t* c)
        cardClone:
            push rbp
            mov rbp, rsp
            push rbx
            push r12

            mov rbx, rdi
       ; creo una carta nueva
            mov rdi, [rbx]
            mov rsi, [rbx + 8]
            call cardNew
            mov r12, rax

       ; borro su lista vacía porque le copiaremos la lista de la carta original 
       		mov rdi, [r12 + 16] 
       		call listDelete

       ; copio las cartas apiladas
            mov rdi, [rbx + 16]
            call listClone
            mov [r12 + 16], rax
        
            mov rax, r12            

            pop r12
            pop rbx
            pop rbp
        ret

        ; void cardAddStacked(card_t* c, card_t* card)
        cardAddStacked:                    
            mov rdi, [rdi + 16]
            call listAddFirst
        ret

        ; void cardDelete(card_t* c)
        cardDelete:
            push rbp
            mov rbp, rsp
            push rbx
            sub rsp, 8          

            mov rbx, rdi

            mov rdi, [rbx + 16]
            call listDelete

            mov rdi, [rbx]
            call strDelete

            mov rdi, [rbx + 8]
            call intDelete

            mov rdi, rbx
            call free            

            add rsp, 8            
            pop rbx
            pop rbp        
        ret

        ; void cardPrint(card_t* c, FILE* pFile)
        cardPrint:
        push rbp
        mov rbp, rsp
        push rbx
        push r12

        mov rbx, rdi
        mov r12, rsi

        mov rdi, llaveizq
        call strPrint

        mov rdi, [rbx]
        mov rsi, r12
        call strPrint

        mov rdi, guion
        mov rsi, r12
        call strPrint

        mov rdi, [rbx+8]
        mov rsi, r12
        call intPrint

        mov rdi, guion
        mov rsi, r12
        call strPrint

        mov rdi, [rbx+16]
        mov rsi, r12
        call listPrint

        mov rdi, llaveder
        mov rsi, r12
        call strPrint

        pop r12
        pop rbx
        pop rbp
	ret
