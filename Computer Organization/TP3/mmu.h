/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

#define MMU_FLAG_PRESENT 1
#define MMU_FLAG_SUPERVISOR 0
#define MMU_FLAG_USER 1
#define MMU_FLAG_READWRITE 1
#define MMU_FLAG_READ 0


typedef struct str_page_directory_entry {
  uint8_t present:1;
  uint8_t read_write:1;
  uint8_t user_supervisor:1;
  uint8_t page_write_through:1;
  uint8_t page_cache_disable:1;
  uint8_t accessed:1;
  uint8_t x:1;
  uint8_t page_size:1;
  uint8_t ignored:1;
  uint8_t available:3;
  uint32_t page_table_base:20;
} __attribute__((__packed__)) page_directory_entry;

typedef struct str_page_table_entry {
  uint8_t present:1;
  uint8_t read_write:1;
  uint8_t user_supervisor:1;
  uint8_t page_write_through:1;
  uint8_t page_cache_disable:1;
  uint8_t accessed:1;
  uint8_t dirty:1;
  uint8_t x:1;
  uint8_t global:1;
  uint8_t available:3;
  uint32_t physical_address_base:20;
} __attribute__((__packed__)) page_table_entry;

void mmu_init(void);

paddr_t mmu_next_free_kernel_page(void);

paddr_t mmu_next_free_user_page(void);

//void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs);
void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint8_t pag_us, uint8_t pag_w, uint8_t pag_p);

paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt);

paddr_t mmu_init_kernel_dir(void);

paddr_t mmu_init_task_dir(paddr_t phy_start);

void lemmings_map_page(uint32_t idx_lemming, paddr_t virt);

void lemmings_unmap_team_pages(uint32_t idx_lemming);

uint32_t check_page_fault(uint32_t virt);

uint32_t check_error_code(uint32_t exception);

#endif //  __MMU_H__
