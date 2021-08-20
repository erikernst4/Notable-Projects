/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "idt.h"

idt_entry_t idt[255] = {0};

idt_descriptor_t IDT_DESC = {sizeof(idt) - 1, (uint32_t)&idt};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);
        ...
    }
*/

#define CODE_SEL_0 GDT_IDX_C0_DESC << 3
#define INTERRUPT_ATTRS 0x8E00
#define SYSCALL_ATTRS   0xEE00 // 1 | 11 | 01110 | 000 | 00000

#define IDT_ENTRY(numero) \
    idt[numero].offset_15_0 = (uint16_t) ((uint32_t)(&_isr ## numero) & (uint32_t) 0xFFFF); \
    idt[numero].segsel = (uint16_t) CODE_SEL_0; \
    idt[numero].attr = (uint16_t) INTERRUPT_ATTRS; \
    idt[numero].offset_31_16 = (uint16_t) ((uint32_t)(&_isr ## numero) >> 16 & (uint32_t) 0xFFFF);

#define IDT_SYSCALL(numero) \
    idt[numero].offset_15_0 = (uint16_t) ((uint32_t)(&_isr ## numero) & (uint32_t) 0xFFFF); \
    idt[numero].segsel = (uint16_t) CODE_SEL_0; \
    idt[numero].attr = (uint16_t) SYSCALL_ATTRS; \
    idt[numero].offset_31_16 = (uint16_t) ((uint32_t)(&_isr ## numero) >> 16 & (uint32_t) 0xFFFF);

void idt_init() {
  IDT_ENTRY(0);
  IDT_ENTRY(1);
  IDT_ENTRY(2);
  IDT_ENTRY(3);
  IDT_ENTRY(4);
  IDT_ENTRY(5);
  IDT_ENTRY(6);
  IDT_ENTRY(7);
  IDT_ENTRY(8);
  IDT_ENTRY(10);
  IDT_ENTRY(11);
  IDT_ENTRY(12);
  IDT_ENTRY(13);
  IDT_ENTRY(14);
  IDT_ENTRY(16);
  IDT_ENTRY(17);
  IDT_ENTRY(18);
  IDT_ENTRY(19);
  IDT_ENTRY(20);
  IDT_ENTRY(21);
  IDT_ENTRY(32);
  IDT_ENTRY(33);
  IDT_SYSCALL(88);
  IDT_SYSCALL(98);
  IDT_SYSCALL(108);
}

