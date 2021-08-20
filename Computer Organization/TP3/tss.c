/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de estructuras para administrar tareas
*/

#include "tss.h"

tss_t tss_initial;
tss_t tss_idle;
tss_t tareas_user[10];
uint32_t esp0_stack_base[10];

void fill_tss_base(tss_t* tss, uint8_t gdt_index) {
  gdt[gdt_index].base_15_0 = ((uintptr_t) tss) & 0x0000FFFF;
  gdt[gdt_index].base_23_16 = ((uintptr_t) tss) & 0x00FF0000;
  gdt[gdt_index].base_31_24 = ((uintptr_t) tss) & 0xFF000000;
}

void fill_user_gdt(uint16_t gdt_index) {
  gdt[gdt_index].limit_15_0 = sizeof(tss_t) - 1;
  gdt[gdt_index].type = 0x9;
  gdt[gdt_index].s = 0x00;
  gdt[gdt_index].dpl = 0x00;
  gdt[gdt_index].p = 0x01;
  gdt[gdt_index].limit_19_16 = 0x00;
  gdt[gdt_index].avl = 0x0;
  gdt[gdt_index].l = 0x0;
  gdt[gdt_index].db = 0x0;
  gdt[gdt_index].g = 0x00;
}

tss_t* make_user_tss(paddr_t start_addr, uint16_t gdt_index) {
  tss_t task = {0};
  task.ss = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  task.ds = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  task.es = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  task.fs = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  task.gs = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  task.cs = GDT_OFF_C3_DESC | GDT_RPL_RING_3;
  task.cr3 = mmu_init_task_dir(start_addr);
  task.eip = TASK_CODE_VIRTUAL;
  task.esp = TASK_STACK_BASE;
  task.ebp = TASK_STACK_BASE;
  //breakpoint();
  task.esp0 = mmu_next_free_kernel_page() + 0x1000;
  task.ss0 = GDT_OFF_D0_DESC;
  task.eflags = 0x00000202;
  tareas_user[gdt_index - GDT_USERS_TSS_INDEX_START] = task;
  esp0_stack_base[gdt_index - GDT_USERS_TSS_INDEX_START] = task.esp0;
  fill_user_gdt(gdt_index);
  fill_tss_base(&tareas_user[gdt_index - GDT_USERS_TSS_INDEX_START], gdt_index);
  return &tareas_user[gdt_index - GDT_USERS_TSS_INDEX_START];
}

void tss_init() {
  tss_idle.ss = GDT_OFF_D0_DESC;
  tss_idle.ds = GDT_OFF_D0_DESC;
  tss_idle.es = GDT_OFF_D0_DESC;
  tss_idle.fs = GDT_OFF_D0_DESC;
  tss_idle.gs = GDT_OFF_D0_DESC;
  tss_idle.cs = GDT_OFF_C0_DESC;
  tss_idle.cr3 = rcr3();
  tss_idle.eip = TASK_IDLE_START;
  tss_idle.esp = mmu_next_free_kernel_page() + 0x1000;
  tss_idle.ebp = tss_idle.esp;
  tss_idle.eflags = 0x00000202;

  fill_tss_base(&tss_idle, GDT_IDX_TASK_IDLE_DESC);
  // No se si hace falta esto pero lo dejo por las dudas. Sino consultar
  fill_tss_base(&tss_initial, GDT_IDX_TASK_INIT_DESC);
}

void reset_tss(tss_t* tss, uint8_t id_lemming){
  tss->eip = TASK_CODE_VIRTUAL;
  tss->esp = TASK_STACK_BASE;
  tss->ebp = TASK_STACK_BASE;
  tss->esp0 = esp0_stack_base[id_lemming];
  tss->eflags = 0x00000202;
  tss->ss = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  tss->ds = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  tss->es = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  tss->fs = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  tss->gs = GDT_OFF_D3_DESC | GDT_RPL_RING_3;
  tss->cs = GDT_OFF_C3_DESC | GDT_RPL_RING_3;
}

