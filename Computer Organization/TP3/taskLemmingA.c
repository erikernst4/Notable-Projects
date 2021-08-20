#include "syscall.h"
#include "types.h"

#define TASK_SHARED_MEM_START 0x00400000
#define ADD_FETCH(X, D) __atomic_add_fetch((X), (D), __ATOMIC_SEQ_CST)

// Chequear que el resultado de una operacion sea esperado.
// En caso de que falle, se generara un breakpoint.
#define CHECK_EQ(A, B)                                                         \
  do {                                                                         \
    if ((A) != (B)) {                                                          \
      __asm volatile("xchg %bx, %bx");                                         \
    }                                                                          \
  } while (0)

// Hacer que el Lemming produzca una excepcion.
#define SEPPUKKU(...)                                                          \
  do {                                                                         \
    __asm volatile("ud2");                                                     \
  } while (0)

void task(void) {
  uint32_t *shared_memory = (uint32_t *)TASK_SHARED_MEM_START;
  CHECK_EQ(shared_memory[0], 0x0);
  uint32_t val = ADD_FETCH(&shared_memory[1], 1);

  // Tengo que desplazarme (val, val) hacia arriba y la derecha.
  for (uint32_t i = 0; i < val; i++) {
    syscall_move(UP);
  }
  for (uint32_t i = 0; i < val; i++) {
    syscall_move(RIGHT);
  }

  while (1) {
    __asm volatile("pause");
  }
}
