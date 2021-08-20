
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper/imagenes.h"
#include "helper/libbmp.h"
#include "helper/tiempo.h"
#include "helper/utils.h"
#include "tp2.h"

// ~~~ seteo de los filtros ~~~

extern filtro_t Gamma;
extern filtro_t Max;
extern filtro_t Max2;
extern filtro_t Funny;
extern filtro_t Broken;
extern filtro_t BrokenExp;
extern filtro_t Max_v;

filtro_t filtros[7];

// ~~~ fin de seteo de filtros ~~~

int main(int argc, char **argv) {

  filtros[0] = Gamma;
  filtros[1] = Max;
  filtros[2] = Max2;
  filtros[3] = Funny;
  filtros[4] = Broken;
  filtros[5] = BrokenExp;
  filtros[6] = Max_v;

  configuracion_t config;
  config.dst.width = 0;
  config.bits_src = 32;
  config.bits_dst = 32;

  procesar_opciones(argc, argv, &config);

  // Imprimo info
  if (!config.nombre) {
    printf("Procesando...\n");
    printf("  Filtro             : %s\n", config.nombre_filtro);
    printf("  Implementación     : %s\n", C_ASM((&config)));
    printf("  Archivo de entrada : %s\n", config.archivo_entrada);
  }

  snprintf(config.archivo_salida, sizeof(config.archivo_salida),
           "%s/%s.%s.%s%s.bmp", config.carpeta_salida,
           basename(config.archivo_entrada), config.nombre_filtro,
           C_ASM((&config)), config.extra_archivo_salida);

  if (config.nombre) {
    printf("%s\n", basename(config.archivo_salida));
    return 0;
  }

  filtro_t *filtro = detectar_filtro(&config);

  filtro->leer_params(&config, argc, argv);
  char filename[80];
  strcpy(filename, "../python/resultados/");
  strcat(filename, filtro->nombre);
  strcat(filename, "_");
  strcat(filename, C_ASM((&config)));
  strcat(filename, "_tiempos.csv");
  FILE *tiemposFile = fopen(filename, "w");
  correr_filtro_imagen(&config, filtro->aplicador, tiemposFile);
  filtro->liberar(&config);

  return 0;
}

filtro_t *detectar_filtro(configuracion_t *config) {
  for (int i = 0; filtros[i].nombre != 0; i++) {
    if (strcmp(config->nombre_filtro, filtros[i].nombre) == 0)
      return &filtros[i];
  }
  fprintf(stderr, "Filtro '%s' desconocido\n", config->nombre_filtro);
  exit(EXIT_FAILURE);
  return NULL;
}

void imprimir_tiempos_ejecucion(unsigned long long int cant_ciclos,
                                int cant_iteraciones) {
  printf("Tiempo de ejecución:\n");
  printf("  # iteraciones                     : %d\n", cant_iteraciones);
  printf("  # de ciclos insumidos totales     : %llu\n", cant_ciclos);
  printf("  # de ciclos insumidos por llamada : %.4f\n",
         (double)cant_ciclos / (double)cant_iteraciones);
}

void correr_filtro_imagen(configuracion_t *config, aplicador_fn_t aplicador, FILE *tiemposFile) {
  imagenes_abrir(config);

  unsigned long long start, end;

  imagenes_flipVertical(&config->src, src_img);
  imagenes_flipVertical(&config->dst, dst_img);
  if (config->archivo_entrada_2 != 0) {
    imagenes_flipVertical(&config->src_2, src_img2);
  }
  unsigned long long elapsedTotal = 0;
  for (int i = 0; i < config->cant_iteraciones; i++) {
    MEDIR_TIEMPO_START(start)
    aplicador(config);
    MEDIR_TIEMPO_STOP(end)
    unsigned long long elapsed = end - start;
    elapsedTotal += elapsed;
    fprintf(tiemposFile, "%llu\n", elapsed);
  }
  imagenes_flipVertical(&config->dst, dst_img);

  imagenes_guardar(config);
  imagenes_liberar(config);
  imprimir_tiempos_ejecucion(elapsedTotal, config->cant_iteraciones);
}
