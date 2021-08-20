/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones de impresion por pantalla.
*/

#include "screen.h"
#include "i386.h"
#include "sched.h"

uint32_t get_num_size(uint32_t numero) {
  if (numero == 0) {
    return 1;
  }
  uint32_t size = 0;
  while (numero != 0) {
    numero /= 10;
    size++;
  }
  return size;
}

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}


void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}

void print_elemento(uint32_t x, uint32_t y, char c) {
  switch(c) {
    case 'A':
      print_lemming(x, y, 'A');
      break;
    case 'B':
      print_lemming(x, y, 'B');
      break;
    case 'P':
      print_obstaculo(x, y);
      break;
    case 'L':
      print_agua(x, y);
      break;
    case '+':
      print_puente(x, y);
      break;
    case 'X':
      print_demoler_obstaculo(x, y);
      break;
    default:
      print_pasto(x, y);
      break;
  }
}

void initialize_screen(void) {
  screen_draw_box(0, 0, 1, VIDEO_COLS, 0, 0);   // Primera fila en negro
  for (int i = 0; i < VIDEO_JUEGO_FILS; i++) {
    for (int j = 0; j < VIDEO_COLS; j++) {
      char c = mapa_original[i][j];
      print_elemento(j, i+1, c);
    }
  }
  print("Amalin", 9, 42, C_FG_LIGHT_RED);
  print("00", 21, 42, C_FG_LIGHT_RED);
  print("01", 25, 42, C_FG_LIGHT_RED);
  print("02", 29, 42, C_FG_LIGHT_RED);
  print("03", 33, 42, C_FG_LIGHT_RED);
  print("04", 37, 42, C_FG_LIGHT_RED);
  print("00", 41, 42, C_FG_CYAN);
  print("01", 45, 42, C_FG_CYAN);
  print("02", 49, 42, C_FG_CYAN);
  print("03", 53, 42, C_FG_CYAN);
  print("04", 57, 42, C_FG_CYAN);
  print("Betarote", 62, 42, C_FG_CYAN);
  screen_draw_box(44, 9, 3, 10, 0, C_BG_RED); // Cuadrado rojo
  print("0", 11, 45, C_FG_WHITE);
  screen_draw_box(44, 62, 3, 10, 0, C_BG_BLUE); // Cuadrado azul
  print("0", 64, 45, C_FG_WHITE);
}

uint8_t offsets_lemming_clock[10] = {0,0,1,1,2,2,3,3,4,4};

void print_lemming_clock(char clock, uint32_t id_lemming, bool activo) {
  char str[2] = "\0";
  str[0] = clock;
  uint32_t offset = offsets_lemming_clock[id_lemming] * 4 + 20 * (id_lemming % 2);
  uint16_t color;
  if(activo==true){
    color = C_FG_WHITE;
  }else{
    color = C_FG_BLACK;
  }
  print(str, 21 + offset, 45, color);
}

void print_lemming(uint32_t x, uint32_t y, char equipo) {
  if (equipo == 'A') {
    print("A", x, y, 0x4F);
  } else {
    print("B", x, y, 0x3F);
  }
}

void print_pasto(uint32_t x, uint32_t y) {
  print("0", x, y, 0x22);
}

void print_puente(uint32_t x, uint32_t y) {
  print("+", x, y, 0x1F);  
}

void print_agua(uint32_t x, uint32_t y) {
  print("0", x, y, 0x11);
}

void print_obstaculo(uint32_t x, uint32_t y) {
  print("O", x, y, 0x6C);
}

void print_demoler_obstaculo(uint32_t x, uint32_t y) {
  print("X", x, y, 0x62);  
}

/* scancodes:
    0x02 = 2  : 1
    0x03 = 3  : 2
    0x04 = 4  : 3
    0x05 = 5  : 4
    0x06 = 6  : 5
    0x07 = 7  : 6
    0x08 = 8  : 7
    0x09 = 9  : 8
    0x0a = 10 : 9
    0x0b = 11 : 0    
*/
uint8_t numsDec[10] = {1,2,3,4,5,6,7,8,9,0};

void print_num(uint8_t scanCode){
  if (scanCode-2 < 10) { // Pregunto si 0 <= scanCode-2 < 10 (que es lo mismo que 2 <= scanCode < 12)
      print_dec(numsDec[scanCode-2], 1, VIDEO_COLS-2, 0, 0x2);
  }
}

void print_scanCode(uint8_t scanCode){
  print_dec(scanCode, 1, VIDEO_COLS-2, 0, 0x2);
}

void print_exception(uint32_t exceptionNumber) {
  screen_draw_box(0, 0, 1, VIDEO_COLS, 0, 0); // Limpio fila
  char* text = "Excepcion ";
  print(text, 0, 0, 0x2);
  uint32_t size = get_num_size(exceptionNumber);
  print_dec(exceptionNumber, size, 10, 0, C_FG_GREEN);
}

void print_lemming_id(uint32_t id_lemming) {
  screen_draw_box(0, 0, 1, VIDEO_COLS, 0, 0); // Limpio fila
  char* text = "Lemming ";
  print(text, 40, 0, 0x2);
  uint32_t size = get_num_size(id_lemming);
  print_dec(id_lemming, size, 60, 0, C_FG_GREEN);
}

void print_libretas(void){
  print("108/19, 113/19, 284/19 ", 0, 0, 0x2);
}
/*
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push esp
    push eip
    mov eax, %1
    push eax
*/

char* nombres_excepciones[32] = {"Divide-by-zero Error #DE [0]", "Debug #DB [1]", "Non-maskable Interrupt - [2]", "Breakpoint	#BP [3]", "Overflow #OF[4]", "Bound Range Exceeded #BR [5]", "Invalid Opcode #UD [6]", "Device Not Available #NM [7]", "Double Fault #DF [8]", "Coprocessor Segment Overrun - [9]", "Invalid TSS #TS [10]", "Segment Not Present #NP [11]", "Stack-Segment Fault #SS [12]", "General Protection Fault #GP [13]", "Page Fault #PF [14]", "Reserved - [15]", "x87 Floating-Point Exception #MF [16]", "Alignment Check #AC [17]", "Machine Check #MC [18]", "SIMD Floating-Point Exception #XM/#XF [19]", "Virtualization Exception #VE [20]", "Reserved - [21]", "Reserved - [22]", "Reserved - [23]", "Reserved - [24]", "Reserved - [25]", "Reserved - [26]", "Reserved - [27]", "Reserved - [28]", "Reserved - [29]", "Security Exception #SX [30]", "Reserved [31]"};

void print_debug(uint32_t cr0, uint32_t cr2, uint32_t cr3, uint32_t cr4, uint32_t ss, uint32_t cs, uint32_t esp_3, uint32_t esp_2, uint32_t esp_1, uint32_t esp, uint32_t eflags, 
                uint32_t errorCode, uint32_t exceptionNumber, uint32_t eip, uint32_t ebp , uint32_t edi ,uint32_t esi ,
                uint32_t edx,uint32_t ecx, uint32_t ebx, uint32_t eax){
  screen_draw_box(2,11,VIDEO_JUEGO_FILS,50,0,C_FG_BLACK);
  
  print(nombres_excepciones[exceptionNumber], 12, 3,C_FG_GREEN);
  
  print("eax", 13, 7,C_FG_WHITE);
  print_hex(eax, 8, 17, 7,C_FG_GREEN);
  
  print("ebx", 13, 9,C_FG_WHITE);
  print_hex(ebx, 8, 17, 9,C_FG_GREEN);
  
  print("ecx", 13, 11,C_FG_WHITE);
  print_hex(ecx, 8, 17, 11,C_FG_GREEN);
  
  print("edx", 13, 13,C_FG_WHITE);
  print_hex(edx, 8, 17, 13,C_FG_GREEN);
  
  print("esi", 13, 15,C_FG_WHITE);
  print_hex(esi, 8, 17, 15,C_FG_GREEN);
  
  print("edi", 13, 17,C_FG_WHITE);
  print_hex(edi, 8, 17, 17,C_FG_GREEN);
  
  print("ebp", 13, 19,C_FG_WHITE);
  print_hex(ebp, 8, 17, 19,C_FG_GREEN);
  
  print("esp", 13, 21,C_FG_WHITE);
  print_hex(esp, 8, 17, 21,C_FG_GREEN);
  
  print("eip", 13, 23,C_FG_WHITE);
  print_hex(eip, 8, 17, 23,C_FG_GREEN);
  
  print("cs", 14, 25,C_FG_WHITE);
  print_hex(cs, 8, 17, 25,C_FG_GREEN);
  
  print("ds", 14, 27,C_FG_WHITE);
  print_hex(ss, 8, 17, 27,C_FG_GREEN);
  
  print("es", 14, 29,C_FG_WHITE);
  print_hex(ss, 8, 17, 29,C_FG_GREEN);
  
  print("fs", 14, 31,C_FG_WHITE);
  print_hex(ss, 8, 17, 31,C_FG_GREEN);
  
  print("gs", 14, 33,C_FG_WHITE);
  print_hex(ss, 8, 17, 33,C_FG_GREEN);
  
  print("ss", 14, 35,C_FG_WHITE);
  print_hex(ss,8, 17, 35,C_FG_GREEN);
  
  print("eflags", 13, 38,C_FG_WHITE);
  print_hex(eflags, 8, 20, 38,C_FG_GREEN);
  
  print("stack", 32, 20,C_FG_WHITE);
  print_hex(esp_1, 8, 32, 22,C_FG_GREEN);
  print_hex(esp_2, 8, 32, 24,C_FG_GREEN);
  print_hex(esp_3, 8, 32, 26,C_FG_GREEN);
  
  print("cr0", 38, 8,C_FG_WHITE);
  print_hex(cr0, 8, 42, 8,C_FG_GREEN);

  print("cr2", 38, 10,C_FG_WHITE);
  print_hex(cr2, 8,42, 10,C_FG_GREEN);

  print("cr3", 38, 12,C_FG_WHITE);
  print_hex(cr3, 8,42, 12,C_FG_GREEN);

  print("cr4", 38, 14,C_FG_WHITE);
  print_hex(cr4, 8,42, 14,C_FG_GREEN);

  print("err", 38, 16,C_FG_WHITE);
  print_hex(errorCode, 8, 42, 16,C_FG_GREEN);
}
void print_game_over(char equipo) {
  screen_draw_box(0, 0, 41, VIDEO_COLS, 0, 0); // Limpio pantalla de juego
  if (equipo == 'A') {
    print("Ganador: Equipo A", 33, 21, C_FG_LIGHT_RED);
  } else {
    print("Ganador: Equipo B", 33, 21, C_FG_LIGHT_CYAN);
  }
}

void print_spawned(uint32_t spawnedA, uint32_t spawnedB) {
  print_dec(spawnedA, get_num_size(spawnedA), 11, 45, C_FG_WHITE);
  print_dec(spawnedB, get_num_size(spawnedB), 64, 45, C_FG_WHITE);
}
