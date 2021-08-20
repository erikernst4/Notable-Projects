#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "../tp2.h"

void BrokenExp_asm (uint8_t *src, uint8_t *dst, int width, int height,
                      int src_row_size, int dst_row_size);

void BrokenExp_c   (uint8_t *src, uint8_t *dst, int width, int height,
                      int src_row_size, int dst_row_size);

typedef void (BrokenExp_fn_t) (uint8_t*, uint8_t*, int, int, int, int);


void leer_params_BrokenExp(configuracion_t *config, int argc, char *argv[]) {
}

void aplicar_BrokenExp(configuracion_t *config)
{
    BrokenExp_fn_t *BrokenExp = SWITCH_C_ASM( config, BrokenExp_c, BrokenExp_asm );
    buffer_info_t info = config->src;
    BrokenExp(info.bytes, config->dst.bytes, info.width, info.height, 
              info.row_size, config->dst.row_size);
}

void liberar_BrokenExp(configuracion_t *config) {

}

void ayuda_BrokenExp()
{
    printf ( "       * BrokenExp\n" );
    printf ( "           Ejemplo de uso : \n"
             "                         BrokenExp -i c facil.bmp\n" );
}

DEFINIR_FILTRO(BrokenExp,1)

