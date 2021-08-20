#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "../tp2.h"

void Max2_asm (uint8_t *src, uint8_t *dst, int width, int height,
                      int src_row_size, int dst_row_size);

void Max2_c   (uint8_t *src, uint8_t *dst, int width, int height,
                      int src_row_size, int dst_row_size);

typedef void (Max2_fn_t) (uint8_t*, uint8_t*, int, int, int, int);


void leer_params_Max2(configuracion_t *config, int argc, char *argv[]) {
}

void aplicar_Max2(configuracion_t *config)
{
    Max2_fn_t *Max2 = SWITCH_C_ASM( config, Max2_c, Max2_asm );
    buffer_info_t info = config->src;
    Max2(info.bytes, config->dst.bytes, info.width, info.height, 
              info.row_size, config->dst.row_size);
}

void liberar_Max2(configuracion_t *config) {

}

void ayuda_Max2()
{
    printf ( "       * Max2\n" );
    printf ( "           Ejemplo de uso : \n"
             "                         Max2 -i c facil.bmp\n" );
}

DEFINIR_FILTRO(Max2,1)


