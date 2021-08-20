; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

%define GDT_SEL_C0_DESC 8<<3
%define GDT_SEL_D0_DESC 10<<3
%define GDT_SEL_INIT_DESC 13<<3
%define GDT_SEL_IDLE_DESC 14<<3

extern GDT_DESC
extern IDT_DESC
extern idt_init
extern pic_reset
extern pic_enable
extern initialize_screen
extern mmu_init_kernel_dir
extern mmu_init
extern mmu_unmap_page
extern mmu_map_page
extern tss_init
extern sched_init
extern game_init

global start


BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0
    

    ; Habilitar A20

    call A20_disable
    call A20_check
    call A20_enable
    call A20_check

    ; Cargar la GDT

    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    ;xchg bx, bx
    mov eax, cr0
    or eax, 1           
    mov cr0, eax

    ; Saltar a modo protegido    
    jmp GDT_SEL_C0_DESC:modoprotegido

BITS 32
modoprotegido:    
            
    ; Establecer selectores de segmentos
    mov ax, GDT_SEL_D0_DESC   
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax 

    ; Establecer la base de la pila
    mov ebp, 0x25000
    mov esp, 0x25000
    
    ; Imprimir mensaje de bienvenida
    print_text_pm start_rm_msg, start_rm_len, 0x07, 0, 0
    
    ; Inicializar pantalla
    call initialize_screen
 
    ; Inicializar el manejador de memoria
 
    ; Inicializar el directorio de paginas

    call mmu_init_kernel_dir

    ; Cargar directorio de paginas
    
    mov cr3, eax

    ; Habilitar paginacion

    mov eax, cr0
    or eax, 1 << 31     
    mov cr0, eax
    call mmu_init

    ; test paginacion
    ;xchg bx, bx
    mov eax, cr3
    push 1      ; present
    push 0      ; user/supervisor
    push 1      ; read/write
    push 0x00400000
    push 0x005de000
    push eax
    call mmu_map_page
    add esp, 6*4
    push 0x005de000
    mov eax, cr3
    push eax    
    call mmu_unmap_page
    add esp, 4

    ; Inicializar tss
    ;xchg bx, bx
    call tss_init

    ; Inicializar el scheduler
    call sched_init

    ; Inicializar la IDT
    call idt_init
    
    ; Cargar IDT
    lidt [IDT_DESC]
 
    ; Configurar controlador de interrupciones
    call pic_reset
    call pic_enable

    ; Ejemplo excepcion: dividir por cero
    ; sti
    ; xor eax, eax
    ; idiv eax

    ; Creo un lemming, inicializo otras cosas
    ;xchg bx, bx
    call game_init

    ; Cargar tarea inicial
    mov ax, GDT_SEL_INIT_DESC
    ltr ax
    ;xchg bx, bx
    ; Saltar a la primera tarea: Idle
    ;xchg bx, bx 
    jmp GDT_SEL_IDLE_DESC:0

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
