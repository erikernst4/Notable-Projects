/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_JUEGO_FILS 40
#define VIDEO_COLS 80

#include "colors.h"
#include "defines.h"
#include "stdint.h"
#include "mapa.h"
#include "types.h"

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
  uint8_t c;
  uint8_t a;
} ca;

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr);

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr);

void screen_draw_layout(void);

void initialize_screen(void);

void print_elemento(uint32_t x, uint32_t y, char c);

void print_lemming(uint32_t x, uint32_t y, char equipo);

void print_lemming_clock(char clock, uint32_t id_lemming, bool activo);

void print_spawned(uint32_t spawnedA, uint32_t spawnedB);

void print_pasto(uint32_t x, uint32_t y);

void print_puente(uint32_t x, uint32_t y);

void print_agua(uint32_t x, uint32_t y);

void print_obstaculo(uint32_t x, uint32_t y);

void print_demoler_obstaculo(uint32_t x, uint32_t y);

void print_exception(uint32_t exceptionNumber);

void print_libretas(void);

void print_debug(uint32_t cr0, uint32_t cr2, uint32_t cr3, uint32_t cr4, uint32_t ss, uint32_t cs, uint32_t esp_3, uint32_t esp_2, uint32_t esp_1, uint32_t esp, uint32_t eflags, 
                uint32_t errorCode, uint32_t exceptionNumber, uint32_t eip, uint32_t ebp , uint32_t edi ,uint32_t esi ,
                uint32_t edx,uint32_t ecx, uint32_t ebx, uint32_t eax);

void print_num(uint8_t scanCode);

void print_scanCode(uint8_t scanCode);

void print_game_over(char ganador);

void print_lemming_id(uint32_t id_lemming); 

#endif //  __SCREEN_H__
