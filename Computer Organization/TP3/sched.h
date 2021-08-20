/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler.
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "types.h"
#include "defines.h"
#include "tss.h"
#include "i386.h"
#include "screen.h"
#include "game.h"

typedef struct str_lemming {
  bool activo;
  char equipo;
  char background;
  char clock;
  uint8_t id;
  int x;
  int y;
  uint16_t gdt_request;
  uint32_t antiguedad;
  tss_t* tss;
} lemming_t;

void sched_init();
uint16_t sched_next_task();
bool sched_activar_lemming_faltante(char equipo);
void sched_desalojar(uint8_t tarea_i);
uint8_t sched_cantidad_lemmings(char equipo);
void sched_reiniciar_lemming_mas_viejo();
lemming_t* sched_devolver_lemming_actual();
lemming_t* sched_devolver_iesimo_lemming(uint8_t i);
uint8_t sched_devolver_indice_actual(void);
void sched_marcar_idle();
void sched_update_lemmings_vivos();
uint8_t get_lemming(uint8_t x, uint8_t y);
void sched_reduce_counter_lemming(char equipo);
void sched_init_lemming(uint8_t id);
#endif //  __SCHED_H__
