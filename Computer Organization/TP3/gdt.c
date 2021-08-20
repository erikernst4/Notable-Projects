/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de la tabla de descriptores globales
*/

#include "gdt.h"
#include "tss.h"

gdt_entry_t gdt[GDT_COUNT] = {
    /* Descriptor nulo*/
    /* Offset = 0x00 */
    [GDT_IDX_NULL_DESC] =
        {
            .limit_15_0 = 0x0000,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .type = 0x0,
            .s = 0x00,
            .dpl = 0x00,
            .p = 0x00,
            .limit_19_16 = 0x00,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0,
            .g = 0x00,
            .base_31_24 = 0x00,
        },
        // Segmento de codigo con nivel 0
        // Arranca en posicion 0x00000008
        // Tamaño de la gdt = (limíte + 1) * granularity
        // 817mB = 856686592 B = (límite + 1) * 4096
        // => límite = (856686592 B / 4096) - 1 = limite = 209151 = 0x3 30ff
        [GDT_IDX_C0_DESC] = {
            .limit_15_0 = 0x30ff,
            .limit_19_16 = 0x3,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,            
            .type = 0x8,
            .dpl = 0x00,

            .s = 0x01,            
            .p = 0x01,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1,
            .g = 0x01,
        },
        // Segmento de codigo con nivel 3
        [GDT_IDX_C3_DESC] = {
            .limit_15_0 = 0x30ff,
            .limit_19_16 = 0x3,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,           
            .type = 0xa,
            .dpl = 0x03,

            .s = 0x01,            
            .p = 0x01,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1,
            .g = 0x01,
        },
        // Segmento de datos con nivel 0
        [GDT_IDX_D0_DESC] = {
            .limit_15_0 = 0x30ff,
            .limit_19_16 = 0x3,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,            
            .type = 0x2,
            .dpl = 0x00,
            
            .s = 0x01,
            .p = 0x01,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1,
            .g = 0x01,
        },
        // Segmento de datos con nivel 3
        [GDT_IDX_D3_DESC] = {
            .limit_15_0 = 0x30ff,
            .limit_19_16 = 0x3,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,           
            .type = 0x2,
            .dpl = 0x03,
            
            .s = 0x01,
            .p = 0x01,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1,
            .g = 0x01,
        },
        // Segmento de área de pantalla
        [GDT_IDX_VS_DESC] = {
            .limit_15_0 = 0x1f40, 
            .limit_19_16 = 0x0,
            .base_15_0 = 0x8000,
            .base_23_16 = 0xB,
            .base_31_24 = 0x00,           
            .type = 0x2,
            .dpl = 0x0,
            
            .s = 0x01,
            .p = 0x01,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1,
            .g = 0x00,
        },
        [GDT_IDX_TASK_INIT_DESC] =
        {
            .limit_15_0 = sizeof(tss_t) - 1,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .type = 0x9,
            .s = 0x00,
            .dpl = 0x00,
            .p = 0x01 ,
            .limit_19_16 = 0x00,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0,
            .g = 0x00,
            .base_31_24 = 0x00,
        },
        [GDT_IDX_TASK_IDLE_DESC] =
        {
            .limit_15_0 = sizeof(tss_t) - 1,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .type = 0x9,
            .s = 0x00,
            .dpl = 0x00,
            .p = 0x01 ,
            .limit_19_16 = 0x00,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0,
            .g = 0x00,
            .base_31_24 = 0x00,
        }
};

gdt_descriptor_t GDT_DESC = {sizeof(gdt) - 1, (uint32_t)&gdt};
