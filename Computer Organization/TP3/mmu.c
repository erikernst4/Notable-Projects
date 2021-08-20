/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "tss.h"
#include "sched.h"
#include "kassert.h"

#define FREE_KERNEL_PAGES_START (0x00100000)
#define FREE_TASK_PAGES_START (0x00400000)

paddr_t next_free_kernel_page;
paddr_t next_free_user_page;
uint32_t err_code[32] = {0};
bool virtA[4096] = {false};
bool virtB[4096] = {false};
paddr_t* phyA;
paddr_t* phyB;

void mmu_init(void) {
  phyA = (paddr_t*) mmu_next_free_kernel_page();
  for (int i = 0; i < 3; i++) {
    mmu_next_free_kernel_page();
  } 
  phyB = (paddr_t*) mmu_next_free_kernel_page();
  for (int i = 0; i < 3; i++) {
    mmu_next_free_kernel_page();
  }
    
  err_code[8]=1;
  err_code[10]=1;
  err_code[11]=1;
  err_code[12]=1;
  err_code[13]=1;
  err_code[14]=1;
  err_code[17]=1;
  err_code[30]=1;

  next_free_kernel_page = FREE_KERNEL_PAGES_START;
  next_free_user_page = FREE_TASK_PAGES_START;
}

paddr_t mmu_next_free_kernel_page(void) {
  paddr_t free_page = next_free_kernel_page;
  next_free_kernel_page += PAGE_SIZE;
  return free_page;
}

paddr_t mmu_next_free_user_page(void) {
  paddr_t free_page = next_free_user_page;
  next_free_user_page += PAGE_SIZE;
  return free_page;
}

paddr_t mmu_init_kernel_dir(void) {
  
  page_directory_entry *pd = (page_directory_entry *) KERNEL_PAGE_DIR;
  page_table_entry *pt_0 = (page_table_entry *) KERNEL_PAGE_TABLE_0;

  // Inicializamos todo en 0

  for (int i = 0; i < 1024; i++) {
    pd[i] = (page_directory_entry){0};
    pt_0[i] = (page_table_entry){0};
  }

  pd[0].present = MMU_FLAG_PRESENT;
  pd[0].read_write = MMU_FLAG_READWRITE;
  pd[0].user_supervisor = MMU_FLAG_SUPERVISOR;
  pd[0].page_table_base = ((uint32_t) pt_0 >> 12);

  for (int i = 0; i < 1024; i++) {
    pt_0[i].present = MMU_FLAG_PRESENT;
    pt_0[i].read_write = MMU_FLAG_READWRITE;
    pt_0[i].user_supervisor = MMU_FLAG_SUPERVISOR;
    pt_0[i].physical_address_base = i;
  }

  return (paddr_t)pd;

}

// void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) {

void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, 
                  uint8_t pag_us, uint8_t pag_w, uint8_t pag_p) {

  page_directory_entry *pd = (page_directory_entry *) cr3;  // Puntero a page directory
  uint32_t directory_idx = virt >> 22;                      // Indice de la direccion virtual en el page directory
  uint32_t table_idx = (virt >> 12) & 0x3FF;                // Indice de la direccion virtual en el page table
  paddr_t table_base_dir = 0;                               // Direccion base de la page table

  if(pd[directory_idx].present != MMU_FLAG_PRESENT) {
    page_table_entry *newPT = (page_table_entry *) mmu_next_free_kernel_page();
    for (size_t i = 0; i < 1024; i++) {
        newPT[i] =(page_table_entry){0};
      }
    pd[directory_idx].present = MMU_FLAG_PRESENT;
    pd[directory_idx].read_write = MMU_FLAG_READWRITE;
    pd[directory_idx].user_supervisor = MMU_FLAG_USER;      // Creo siempre con permisos de usuario
    pd[directory_idx].page_table_base = ((uint32_t) newPT >> 12);
  }

  table_base_dir = (pd[directory_idx].page_table_base) << 12;
  page_table_entry *pt = (page_table_entry *) table_base_dir;

  pt[table_idx].present = pag_p;
  pt[table_idx].read_write = pag_w;
  pt[table_idx].user_supervisor = pag_us;
  pt[table_idx].physical_address_base = (phy >> 12);
  
  tlbflush();

}


paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt) {

  page_directory_entry *pd = (page_directory_entry *) cr3;  // Puntero a page directory
  uint32_t directory_idx = virt >> 22;                      // Indice de la direccion virtual en el page directory
  uint32_t table_idx = (virt >> 12) & 0x3FF;                // Indice de la direccion virtual en el page table
  paddr_t table_base_dir = 0;                               // Direccion base de la page table

  if(pd[directory_idx].present == 1) {
    table_base_dir = (pd[directory_idx].page_table_base) << 12;
    page_table_entry *pt = (page_table_entry *) table_base_dir;
    pt[table_idx] = (page_table_entry){0};
    tlbflush(); 
  }

  return (paddr_t)pd;
}

paddr_t mmu_init_task_dir(paddr_t phy_start) {


  page_directory_entry *pd = (page_directory_entry *) mmu_next_free_kernel_page();
  page_table_entry *pt_0 = (page_table_entry *) mmu_next_free_kernel_page();

  // Inicializamos todo en 0

  for (int i = 0; i < 1024; i++) {
    pd[i] = (page_directory_entry){0};
    pt_0[i] = (page_table_entry){0};
  }

  pd[0].present = MMU_FLAG_PRESENT;
  pd[0].read_write = MMU_FLAG_READWRITE;
  pd[0].user_supervisor = MMU_FLAG_SUPERVISOR;
  pd[0].page_table_base = ((uint32_t) pt_0 >> 12);

  for (int i = 0; i < 1024; i++) {
    pt_0[i].present = MMU_FLAG_PRESENT;
    pt_0[i].read_write = MMU_FLAG_READWRITE;
    pt_0[i].user_supervisor = MMU_FLAG_SUPERVISOR;
    pt_0[i].physical_address_base = i;
  }

  mmu_map_page((uint32_t)pd,0x08000000,phy_start,MMU_FLAG_USER,MMU_FLAG_READ,MMU_FLAG_PRESENT);
  mmu_map_page((uint32_t)pd,0x08001000,phy_start+PAGE_SIZE,MMU_FLAG_USER,MMU_FLAG_READ,MMU_FLAG_PRESENT);

  mmu_map_page((uint32_t)pd,0x08002000,mmu_next_free_user_page(),MMU_FLAG_USER,MMU_FLAG_READWRITE,MMU_FLAG_PRESENT);
  
  return (paddr_t)pd;

}

// Para el lemming idx_lemming, hace un mapeo nuevo en todos los lemmings de ese equipo activos
// y actualiza las estructuras


void lemmings_map_page(uint32_t idx_lemming, paddr_t virt){
  //breakpoint();
  paddr_t phy = 0;
  uint32_t idx = (virt - 0x400000) >> 12;
  // Si es par, mapeo los pares porque son los del equipo A, caso contrario mapeo los impares del equipo B
  if (idx_lemming % 2 == 0) {
    if (virtA[idx]){
      phy = phyA[idx];
    }else{
      phy = mmu_next_free_user_page();
      virtA[idx] = true;
      phyA[idx] = phy;
    }
  } else {
    if (virtB[idx]){
      phy = phyB[idx];
    }else{
      phy = mmu_next_free_user_page();
      virtB[idx] = true;
      phyB[idx] = phy;
    }
  }
  uint32_t tss_base_0_16 = 0;
  uint32_t tss_base_23_16 = 0;
  uint32_t tss_base_31_24 = 0;
  tss_base_0_16 = gdt[idx_lemming].base_15_0;
  tss_base_23_16 = gdt[idx_lemming].base_23_16;
  tss_base_31_24 = gdt[idx_lemming].base_31_24;
  tss_t* tss_base = (tss_t *) ((tss_base_31_24 << 24) | (tss_base_23_16 << 16) | tss_base_0_16);
  uint32_t cr3 = tss_base->cr3;
  //breakpoint();
  mmu_map_page(cr3, virt, phy, MMU_FLAG_USER, MMU_FLAG_READWRITE, MMU_FLAG_PRESENT);
}

// Cuando matas al lemming idx_lemming le desmapeas todas las que estaba usando
// es decir las virtuales correspondiente a su equipo

void lemmings_unmap_team_pages(uint32_t idx_lemming){
  uint32_t tss_base_0_16 = 0;
  uint32_t tss_base_23_16 = 0;
  uint32_t tss_base_31_24 = 0;
  tss_base_0_16 = gdt[GDT_USERS_TSS_INDEX_START+idx_lemming].base_15_0;
  tss_base_23_16 = gdt[GDT_USERS_TSS_INDEX_START+idx_lemming].base_23_16;
  tss_base_31_24 = gdt[GDT_USERS_TSS_INDEX_START+idx_lemming].base_31_24;
  tss_t* tss_base = (tss_t *) ((tss_base_31_24 << 24) | (tss_base_23_16 << 16) | tss_base_0_16);
  uint32_t cr3 = tss_base->cr3;
  //breakpoint();
  for (uint32_t i = 0; i < 4096; i++) {
    if(idx_lemming % 2 == 0 && virtA[i]){
      mmu_unmap_page(cr3, (i<<12) + 0x400000);
      if (sched_cantidad_lemmings('A')==1){ 
        virtA[i]=false;
      }
    } else if (idx_lemming % 2 != 0 && virtB[i]) {
      mmu_unmap_page(cr3, (i<<12) + 0x400000);
      if (sched_cantidad_lemmings('B')==1){ 
        virtB[i]=false;
      }    
    }
  }
}

uint32_t check_page_fault(uint32_t virt){
  uint32_t res = 0;
  if (virt >= (0x00400000) && virt <= (0x013FFFFF)){
    res = 1;
  }
  return res;
}



uint32_t check_error_code(uint32_t exception) {
  return err_code[exception];
}
