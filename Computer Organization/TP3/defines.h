/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__
/* MMU */
/* -------------------------------------------------------------------------- */

#define MMU_P (1 << 0)
#define MMU_W (1 << 1)
#define MMU_U (1 << 2)

#define PAGE_SIZE 4096

/* Misc */
/* -------------------------------------------------------------------------- */
// Y Filas
#define SIZE_N 40
#define ROWS   SIZE_N

// X Columnas
#define SIZE_M 80
#define COLS   SIZE_M

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_IDX_NULL_DESC 0
#define GDT_IDX_C0_DESC 8
#define GDT_IDX_C3_DESC 9
#define GDT_IDX_D0_DESC 10
#define GDT_IDX_D3_DESC 11
#define GDT_IDX_VS_DESC 12
#define GDT_IDX_TASK_INIT_DESC 13
#define GDT_IDX_TASK_IDLE_DESC 14
#define GDT_IDX_TASK_A1_DESC 20
#define GDT_IDX_TASK_B1_DESC 21
#define GDT_IDX_TASK_A2_DESC 22
#define GDT_IDX_TASK_B2_DESC 23
#define GDT_IDX_TASK_A3_DESC 24
#define GDT_IDX_TASK_B3_DESC 25
#define GDT_IDX_TASK_A4_DESC 26
#define GDT_IDX_TASK_B4_DESC 27
#define GDT_IDX_TASK_A5_DESC 28
#define GDT_IDX_TASK_B5_DESC 29
#define GDT_COUNT         35

#define GDT_USERS_TSS_INDEX_START 20
#define GDT_USERS_TSS_INDEX_END 29

#define GDT_RPL_RING_3 0x0003

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC (GDT_IDX_NULL_DESC << 3)
#define GDT_OFF_C0_DESC (GDT_IDX_C0_DESC << 3)
#define GDT_OFF_D0_DESC (GDT_IDX_D0_DESC << 3)
#define GDT_OFF_C3_DESC (GDT_IDX_C3_DESC << 3)
#define GDT_OFF_D3_DESC (GDT_IDX_D3_DESC << 3)
#define GDT_OFF_TASK_INIT_DESC (GDT_IDX_TASK_INIT_DESC << 3)
#define GDT_OFF_TASK_IDLE_DESC (GDT_IDX_TASK_IDLE_DESC << 3)

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */

// direccion fisica de comienzo del bootsector (copiado)
#define BOOTSECTOR 0x00001000
// direccion fisica de comienzo del kernel
#define KERNEL 0x00001200
// direccion fisica del buffer de video
#define VIDEO 0x000B8000

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */

// direccion virtual del codigo
#define TASK_CODE_VIRTUAL 0x08000000
#define TASK_CODE_PAGES   2
#define TASK_STACK_BASE   0x08003000

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<X>_PHY_START.
 */
#define USER_TASK_SIZE (PAGE_SIZE * 2)

#define TASK_LEMMING_A_CODE_START (0x00018000)
#define TASK_LEMMING_B_CODE_START (0x0001A000)
#define TASK_IDLE_START           (0x0001C000)

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR     (0x00025000)
#define KERNEL_PAGE_TABLE_0 (0x00026000)
#define KERNEL_STACK        (0x00025000)

#endif //  __DEFINES_H__
