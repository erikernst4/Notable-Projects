; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32
;; Screen
extern print_exception
extern print_num
extern print_debug
extern print_scanCode

;; PIC
extern pic_finish1

;; Sched
extern sched_next_task
extern sched_desalojar
extern sched_marcar_idle

;; Game
extern game_tick
extern game_move_lemming
extern game_explode_lemming
extern game_bridge_lemming
extern restaurar_mapa
extern game_kill_lemming_actual
TASK_STACK_BASE: dd 0x08003000

;; mmu
extern lemmings_map_page
extern check_page_fault
extern check_error_code

%define GDT_SEL_IDLE_DESC 14<<3
offset: dd 0
selector: dw 0
MODO_DEBUG: DB 0
REANUDACION: DB 0
PAUSA: DB 0

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
%macro ISR 1
global _isr%1

_isr%1:
    ;xchg bx, bx
    pushad
    push ecx
    xor cl, cl
    cmp cl, [MODO_DEBUG]
    pop ecx
    jne .modoDebug
    mov eax, %1
    push eax
    call print_exception
    add esp, 4  ; desapilar eax
    jmp .idle
    
    .modoDebug:
    mov BYTE [PAUSA], 0xFF
    
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp

    mov eax, %1
    push eax
    call check_error_code
    add esp, 4
    cmp eax, 0
    jne .conErrorCode     

    
;sinErrorCode
    ; eip
    mov eax, [esp + 4 * 15]
    push eax
    ; numero de excepción
    mov eax, %1
    push eax
    ; error code (no hay, pusheo 0) 
    xor eax, eax
    push eax
    ; eflags
    mov eax, [esp + 4 * 18 + 8]
    push eax
    ; esp
    mov eax, [esp+ 4 * 19 + 12]
    push eax
    ; Pusheo los primeros 3 elementos de la pila (si los hay)
    mov ebx, [TASK_STACK_BASE]
    sub eax, ebx
    cmp eax, 0
    je .pilaVacia1   
    mov ebx, [esp + 4 * 19 + 12]    
    mov ecx, 4      ; me guardo en ecx cuántos elementos pusheo de la pila para después desapilar
    cmp eax, 4
    je .hay1Elemento
    add ecx, 4    
    cmp eax, 8
    je .hay2Elementos
    add ecx, 4
    ; últimos 3 cosas de la pila
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    mov eax, [ebx+8]
    push eax
    jmp .sigo1
    .hay2Elementos:
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    xor eax, eax
    push eax
    jmp .sigo1
    .hay1Elemento:
    mov eax, [ebx]
    push eax
    xor eax, eax
    push eax
    push eax
    jmp .sigo1
    .pilaVacia1:
    xor eax, eax
    push eax
    push eax
    push eax    
    .sigo1:
    ; cs
    mov eax, [esp + 4 * 23 + 4]
    push eax
    ; ss
    mov eax, [esp + 4 * 24 + 16]
    push eax
    jmp .siguiente

.conErrorCode:
    ; eip
    mov eax, [esp + 4 * 15 + 4]
    push eax
    ; numero de excepción
    mov eax, %1
    push eax
    ; error code 
    mov eax, [esp + 4 * 17]
    push eax
    ; eflags
    mov eax, [esp + 4 * 18 + 12]
    push eax
    ; esp
    mov eax, [esp+ 4 * 19 + 16]
    ;xchg bx, bx
    push eax
    ; Pusheo los primeros 3 elementos de la pila (si los hay)
    ;xchg bx, bx
    mov ebx, [TASK_STACK_BASE]
    sub eax, ebx
    cmp eax, 0
    je .pilaVacia    
    mov ebx, [esp+ 4 * 19 + 16]    
    mov ecx, 4      ; me guardo en ecx cuántos elementos pusheo de la pila para después desapilar
    cmp eax, 4
    je .hayUnSoloElemento
    add ecx, 4    
    cmp eax, 8
    je .hayDosElementos
    add ecx, 4
    ; últimos 3 cosas de la pila
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    mov eax, [ebx+8]
    push eax
    jmp .sigo
    .hayDosElementos:
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    xor eax, eax
    push eax
    jmp .sigo
    .hayUnSoloElemento:
    mov eax, [ebx]
    push eax
    xor eax, eax
    push eax
    push eax
    jmp .sigo
    .pilaVacia:
    xor eax, eax
    push eax
    push eax
    push eax    
    .sigo:
    ; cs
    mov eax, [esp + 4 * 23 + 8]
    push eax
    ; ss
    mov eax, [esp + 4 * 24 + 20]
    push eax
    .siguiente:
    ; control registers
    mov eax, cr4    
    push eax
    mov eax, cr3    
    push eax
    mov eax, cr2    
    push eax
    mov eax, cr0    
    push eax

    ;xchg bx, bx
    call print_debug
    add esp, 21 * 4
    
    .idle:
    mov eax, %1
    push eax    
    call check_error_code
    add esp, 4
    cmp eax, 0
    je .skipErr
    add esp, 4 ; si la excepcion tiene error code la desapilo
    .skipErr:
    ;xchg bx, bx
    call game_kill_lemming_actual
    .fin:
    call sched_marcar_idle
    ;xchg bx, bx
    mov bx, GDT_SEL_IDLE_DESC
    mov [selector], bx
    jmp far [offset]
%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 10
ISR 11
ISR 12
ISR 13
global _isr14
_isr14:
    ;xchg bx, bx
    pushad
    push ecx
    xor cl, cl
    cmp cl, [MODO_DEBUG]
    pop ecx
    jne .modoDebug
    mov eax, 14
    push eax
    call print_exception
    add esp, 4  ; desapilar eax
    jmp .idle
    
    .modoDebug:
    mov BYTE [PAUSA], 0xFF
    
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    
    ; eip
    mov eax, [esp + 4 * 15 + 4]
    push eax
    ; numero de excepción
    mov eax, 14
    push eax
    ; error code 
    mov eax, [esp + 4 * 17]
    push eax
    ; eflags
    mov eax, [esp + 4 * 18 + 12]
    push eax
    ; esp
    mov eax, [esp + 4 * 19 + 16]
    ;xchg bx, bx
    push eax
    ; Pusheo los primeros 3 elementos de la pila (si los hay)
    ;xchg bx, bx
    mov ebx, [TASK_STACK_BASE]
    sub eax, ebx
    cmp eax, 0
    je .pilaVacia    
    mov ebx, [esp + 4 * 19 + 16]    
    mov ecx, 4      ; me guardo en ecx cuántos elementos pusheo de la pila para después desapilar
    cmp eax, 4
    je .hayUnSoloElemento
    add ecx, 4    
    cmp eax, 8
    je .hayDosElementos
    add ecx, 4
    ; últimos 3 cosas de la pila
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    mov eax, [ebx+8]
    push eax
    jmp .sigo
    .hayDosElementos:
    mov eax, [ebx]
    push eax
    mov eax, [ebx+4]
    push eax
    xor eax, eax
    push eax
    jmp .sigo
    .hayUnSoloElemento:
    mov eax, [ebx]
    push eax
    xor eax, eax
    push eax
    push eax
    jmp .sigo
    .pilaVacia:
    xor eax, eax
    push eax
    push eax
    push eax    
    .sigo:
    ; cs
    mov eax, [esp + 4 * 23 + 8]
    push eax
    ; ss
    mov eax, [esp + 4 * 24 + 20]
    push eax
    ; control registers
    mov eax, cr4    
    push eax
    mov eax, cr3    
    push eax
    mov eax, cr2    
    push eax
    mov eax, cr0    
    push eax

    ;xchg bx, bx
    call print_debug
    add esp, 21 * 4
    
    .idle:
    mov eax, cr2
    push eax
    call check_page_fault
    add esp, 4
    cmp eax, 0
    je .dont_handle
    mov eax, cr2
    push eax    
    xor ecx, ecx
    str cx
    shr ecx, 3
    push ecx
    ;xchg bx, bx
    call lemmings_map_page
    ;xchg bx, bx
    add esp, 8
    popad
    add esp, 4 ; desapilo error code
    jmp .finSinFallo
    .dont_handle:
    ;xchg bx, bx
    call game_kill_lemming_actual
    ;mov DWORD [esp + 16], 0x08003000
    ;mov DWORD [esp + 4], 0x08000000
    call sched_marcar_idle
    ;xchg bx, bx
    mov bx, GDT_SEL_IDLE_DESC
    mov [selector], bx
    jmp far [offset]
    ;xchg bx, bx
    popad
    add esp, 4
    .finSinFallo:
    ;xchg bx, bx    
    iret

ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    ;xchg bx, bx
    pushad
    call pic_finish1
    call next_clock
    
    xor bl, bl
    cmp bl, [MODO_DEBUG]
    je .funcionamientoNormal
    
    ; analizo si el juego está pausado
    xor bl, bl
    cmp bl, [PAUSA]
    je .funcionamientoNormal

    ; analizo la reanudación
    xor bl, bl
    cmp bl, [REANUDACION]
    je .fin
    ; apago reanudación para la próxima excepción, restauro el mapa y sigo el juego
    mov BYTE [REANUDACION], 0
    mov BYTE [PAUSA], 0
    call restaurar_mapa
    
    .funcionamientoNormal:
    call game_tick
    call sched_next_task
    str cx
    cmp ax, cx
    je .fin
    
    mov [selector], ax
    jmp far [offset]
    
    .fin:
    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    pushad
    ;xchg bx, bx
    in al, 0x60 	    ; Captura una tecla
    cmp al, 0x95 		; Me fijo si se presionó "y"
    je .debug
    
    push eax
    call print_scanCode
    add esp, 4
    jmp .fin
    
    .debug:
    xor cl, cl
    cmp cl, [MODO_DEBUG]
    jne .posibleReanudacion
    ; Prendo Modo debug
    mov BYTE [MODO_DEBUG], 0xFF
    jmp .fin    

    .posibleReanudacion:
    xor cl, cl
    cmp cl, [PAUSA]
    je .fin
    ; Prendo reanudación
    mov BYTE [REANUDACION], 0xFF
    .fin:
    call pic_finish1
    ; Indica que la interrupcion fue antendida
    popad
iret

;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;

global _isr88
_isr88:
    ;xchg bx, bx
    pushad
    push eax
    call game_move_lemming
    add esp, 4
    mov [esp + 7*4], eax ; piso el eax pusheado con el resultado del move 

;    call sched_marcar_idle
    mov bx, GDT_SEL_IDLE_DESC
    mov [selector], bx
    jmp far [offset]
    popad
    iret

global _isr98
_isr98:
    pushad
    call game_explode_lemming
    
;    call sched_marcar_idle
    mov bx, GDT_SEL_IDLE_DESC
    mov [selector], bx
    jmp far [offset]
    
    popad
    iret

global _isr108
_isr108:
    pushad
    ;xchg bx, bx
    push eax
    call game_bridge_lemming
    add esp, 4

    ;xor cl, cl
    ;div cl

;    call sched_marcar_idle
    mov bx, GDT_SEL_IDLE_DESC
    mov [selector], bx
    jmp far [offset]
    
    popad
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret
