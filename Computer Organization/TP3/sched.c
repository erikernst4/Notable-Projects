/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"

lemming_t lemmings[10] = {0};
int lemming_actual_idx = 0;
bool en_idle = true;
uint8_t cantidad_lemmings_a = 0;
uint8_t cantidad_lemmings_b = 0;

char clock_animacion[4] = { '|', '/', '-', '\\'};

char next_clock_char(char clock) {
  if (clock == ' ') return clock_animacion[0];
  for (int i = 0; i < 4; i++) {
    if (clock == clock_animacion[i]) {
      return clock_animacion[(i+1)%4];
    }
  }
  return ' ';
}

void sched_update_lemmings_vivos() {
  for (int i = 0; i < 10; i++) {
    if (lemmings[i].activo) {
      lemmings[i].antiguedad++;
    }
    /*if (lemming_actual_idx==1 && i == lemming_actual_idx && !en_idle){
      breakpoint();
    }*/
    if (!en_idle && i == lemming_actual_idx) {
      lemmings[i].clock = next_clock_char(lemmings[i].clock);
      print_lemming_clock(lemmings[i].clock, lemmings[i].id,lemmings[i].activo);
    }
  }
}

void sched_marcar_idle() {
  en_idle = true;
}

lemming_t* sched_devolver_lemming_actual() {
  return &lemmings[lemming_actual_idx];
}

lemming_t* sched_devolver_iesimo_lemming(uint8_t i){
  return &lemmings[i];
}

uint8_t sched_devolver_indice_actual(void){
    return lemming_actual_idx;
}

void sched_init_lemming(uint8_t id) {
  lemming_t* lemming = &lemmings[id];
  if (lemming->equipo == 'A') {
      lemming->x = 0;
      lemming->y = 39;
  }
  if (lemming->equipo == 'B') {
      lemming->x = 79;
      lemming->y = 0;
  }
  lemming->activo = false;
  lemming->antiguedad = 0;
  lemming->background = '.';
  lemming->clock = ' ';
}

void sched_init(void) {
  for (uint8_t i = GDT_USERS_TSS_INDEX_START; i <= GDT_USERS_TSS_INDEX_END; i++) {
    vaddr_t task_code_start;
    char equipo;
    // Vamos alternando entre task A y task B para que sea mas comodo para el sched despues
    if (i % 2 == 0) {
      equipo = 'A';
      task_code_start = TASK_LEMMING_A_CODE_START;
    } else {
      equipo = 'B';
      task_code_start = TASK_LEMMING_B_CODE_START;
    }
    lemming_t lemming = {0};
    lemming.tss = make_user_tss(task_code_start, i);
    lemming.gdt_request = (i << 3);
    lemming.equipo = equipo;
    uint8_t index = i - GDT_USERS_TSS_INDEX_START; 
    lemming.id = index;
    lemmings[index] = lemming;
    sched_init_lemming(index);
  }
}

uint16_t sched_next_task(void) {
  if (game_over()) {
    en_idle = true;
    return GDT_OFF_TASK_IDLE_DESC;
  }
  int lemming_previo_idx = lemming_actual_idx;
  // Si esta en idle le pongo una letra cualquiera para decir que no tiene equipo
  char equipo_previo = en_idle ? 'Z' : lemmings[lemming_previo_idx].equipo;
  lemming_actual_idx = (lemming_actual_idx + 1) % 10;
  while (!lemmings[lemming_actual_idx].activo || lemmings[lemming_actual_idx].equipo == equipo_previo) {
    if (lemming_actual_idx == lemming_previo_idx || lemmings[lemming_actual_idx].antiguedad < 1) {
      // no hay ninguna activa que (o ninguna que no haya sido recién creada) del otro equipo, saltar a idle
      en_idle = true;
      return GDT_OFF_TASK_IDLE_DESC;
    }
    lemming_actual_idx = (lemming_actual_idx + 1) % 10;
  }
  en_idle = false;
  return lemmings[lemming_actual_idx].gdt_request;
}

bool sched_activar_lemming_faltante(char equipo) {
  if (equipo != 'A' && equipo != 'B') return false;
  if ((equipo == 'A' && cantidad_lemmings_a >= 5)
      || (equipo == 'B' && cantidad_lemmings_b >= 5)) {
    return false;
  }

  uint16_t i = 0;
  while(lemmings[i].activo || lemmings[i].equipo != equipo) {
    i++;
  }
  if (game_espacio_ocupado(lemmings[i].x, lemmings[i].y)) return false;
  lemmings[i].activo = true;
  reset_tss(lemmings[i].tss, lemmings[i].id);
  print_lemming(lemmings[i].x, lemmings[i].y + 1, lemmings[i].equipo);
  if (equipo == 'A') {
    cantidad_lemmings_a++;
  } else {
    cantidad_lemmings_b++;
  }
  return true;
}

uint8_t sched_cantidad_lemmings(char equipo) {
  return equipo == 'A' ? cantidad_lemmings_a : cantidad_lemmings_b;
}

void sched_desalojar(uint8_t tarea_i){
  game_kill_lemming(tarea_i);
}

void sched_reiniciar_lemming_mas_viejo() {
  uint32_t max_antiguedad = 0;
  int index_max = -1;
  char equipo;
  for (int i = 0; i < 10; i++) {
    if (lemmings[i].activo && lemmings[i].antiguedad >= max_antiguedad) {
      max_antiguedad = lemmings[i].antiguedad;
      index_max = i;
      equipo = lemmings[i].equipo;
    }
  }
  if (index_max != -1) {
    sched_desalojar(index_max);
    sched_activar_lemming_faltante(equipo);
  }
}

uint8_t get_lemming(uint8_t x, uint8_t y){
    for (int i = 0; i < 10; i++) {
        if (lemmings[i].x == x && lemmings[i].y == y) {
          return i;
        }
    }
    return 0;
}

void sched_reduce_counter_lemming(char equipo){
  if (equipo == 'A') {
    if (cantidad_lemmings_a == 0) return; 
    cantidad_lemmings_a--;
  } else {
    if (cantidad_lemmings_b == 0) return;
    cantidad_lemmings_b--;
  }
}
