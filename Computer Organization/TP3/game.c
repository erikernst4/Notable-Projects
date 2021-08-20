/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "i386.h"
#include "tss.h"
size_t count_spawned_a = 0;
size_t count_spawned_b = 0;
uint32_t ticks_passed = 0;
char mapa[40][80];
bool var_game_over = false;
char ultimo_lemming_creado_equipo = 'B'; // Lo inicializo en B, de esta forma primero se crea un lemming A en crear_lemming_faltante

// Cada 401 ciclos de reloj, si algun equipo tiene menos de 5 Lemmings en juego, el sistema creara un nuevo Lemming para ese equipo, 
// posicionandolo en la direccion inicial del mapa para su equipo
static const uint32_t TICK_CREAR_LEMMING = 401;

// Cada 2001 ciclos de reloj, si algún equipo tiene 5 Lemmings en juego, el sistema desalojará al
// Lemming más antiguo y creará uno nuevo, posicionandolo en la dirección inicial del mapa para su
// equipo
static const uint32_t TICK_MATAR_LEMMING = 2001;

bool game_espacio_ocupado(uint32_t x, uint32_t y) {
    return mapa[y][x] == 'A' || mapa[y][x] == 'B' || mapa[y][x] == 'P' || mapa[y][x] == 'L';
}

void game_kill_lemming(uint8_t i) {
    lemming_t* lemming = sched_devolver_iesimo_lemming(i);    
    print_elemento(lemming->x, lemming->y + 1, lemming->background);
    mapa[lemming->y][lemming->x] = lemming->background;
    sched_init_lemming(lemming->id);
    lemmings_unmap_team_pages(lemming->id);
    sched_reduce_counter_lemming(lemming->equipo);
    print_lemming_clock(lemming->clock, lemming->id, false);
}

void game_kill_lemming_actual(void) {
    game_kill_lemming(sched_devolver_indice_actual());

}

void copiar_mapa_original(void) {
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 80; j++) {
            mapa[i][j] = mapa_original[i][j];
        }
    }
}

void restaurar_mapa(void) {
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 80; j++) {
            print_elemento(j, i + 1, mapa[i][j]);
        }
    }
}

bool es_direccion_valida(uint32_t direccion) {
    return direccion <= 3;
}

bool posicion_mapa_valida(int x, int y) {
    return x >= 0 && x < 80 && y < 40 && y >= 0;
}

move_result_e move_result(lemming_t* lemming, direction_e direccion) {
    int new_x = lemming->x;
    int new_y = lemming->y; 
    switch(direccion) {
        case DIR_UP:
            new_y--;
            break;
        case DIR_RIGHT:
            new_x++;
            break;
        case DIR_DOWN:
            new_y++;
            break;
        case DIR_LEFT:
            new_x--;
            break;
    }
    if (!posicion_mapa_valida(new_x, new_y)) {
        return MOVE_RESULT_BORDER;
    }
    char elemento = mapa[new_y][new_x];
    switch (elemento) {
        case 'P':
            return MOVE_RESULT_WALL;
            break;
        case 'L':
            return MOVE_RESULT_LAKE;
            break;
        case 'A':
            return MOVE_RESULT_LEMMING;
            break;
        case 'B':
            return MOVE_RESULT_LEMMING;
            break;
    }
    print_elemento(lemming->x, lemming->y + 1, lemming->background);
    print_lemming(new_x, new_y + 1, lemming->equipo);
    mapa[lemming->y][lemming->x] = lemming->background;
    lemming->x = new_x;
    lemming->y = new_y;
    lemming->background = mapa[new_y][new_x]; 
    mapa[new_y][new_x] = lemming->equipo;
    return MOVE_RESULT_SUCCESS;
}

void check_game_finish(lemming_t* lemming) {
    //breakpoint();
    if (lemming->equipo == 'A' && lemming->x == 79) {
        var_game_over = true;
        print_game_over('A');
    } else if (lemming->equipo == 'B' && lemming->x == 0) {
        var_game_over = true;
        print_game_over('B');
    }
}

bool game_over(void) {
    return var_game_over;
}

move_result_e game_move_lemming(uint32_t direccion) {
    //breakpoint();
    if (!es_direccion_valida(direccion)) {
        game_kill_lemming_actual();
        return -1;
    }
    lemming_t* lemming = sched_devolver_lemming_actual();
    direction_e dir_enum = (direction_e) direccion;
    move_result_e result = move_result(lemming, dir_enum);
    check_game_finish(lemming);
    return result;
}

void demoler_si_valido(int x, int y) {
    if (!posicion_mapa_valida(x, y)) return;
    if (mapa[y][x] == 'P') {
        mapa[y][x] = 'X';
        print_demoler_obstaculo(x, y + 1);
    }
    if (mapa[y][x] == 'A' || mapa[y][x] == 'B'){
        game_kill_lemming(get_lemming(x, y));
    }
}

void game_explode_lemming(void) {
    lemming_t* lemming = sched_devolver_lemming_actual();
    demoler_si_valido((lemming->x) + 1, lemming->y);
    demoler_si_valido((lemming->x) - 1, lemming->y);
    demoler_si_valido(lemming->x, (lemming->y) + 1);
    demoler_si_valido(lemming->x, (lemming->y) - 1);
    game_kill_lemming_actual();
}

void crear_puente_si_valido(lemming_t* lemming, direction_e direccion) {
    int x = lemming->x;
    int y = lemming->y; 
    switch(direccion) {
        case DIR_UP:
            y--;
            break;
        case DIR_RIGHT:
            x++;
            break;
        case DIR_DOWN:
            y++;
            break;
        case DIR_LEFT:
            x--;
            break;
    }
    /*
    if (mapa[y][x] == 'L')
    {
        breakpoint();
    }
    */
    if (posicion_mapa_valida(x, y) && mapa[y][x] == 'L') {
        mapa[y][x] = '+';
        print_puente(x, y+1);
        //breakpoint();
        return;
    }
 //   breakpoint();
}

void game_bridge_lemming(uint32_t direccion) {
    //breakpoint();
    if (!es_direccion_valida(direccion)) {
        game_kill_lemming_actual();
        return;
    } 
    direction_e dir_enum = (direction_e) direccion;
    lemming_t* lemming = sched_devolver_lemming_actual();
    crear_puente_si_valido(lemming, dir_enum);
    game_kill_lemming_actual();
}

bool alguno_menos_lemmings() {
    return sched_cantidad_lemmings('A') < 5 || sched_cantidad_lemmings('B') < 5;
}

void crear_lemming_faltante() {
    //breakpoint();
    if (!alguno_menos_lemmings()) return;
    bool creado = false;
    char equipo;
    if (ultimo_lemming_creado_equipo == 'A') {
        creado = sched_activar_lemming_faltante('B');
        equipo = 'B';
    } else {
        creado = sched_activar_lemming_faltante('A');
        equipo = 'A';
    }
    if (creado) {
        ultimo_lemming_creado_equipo = equipo;
        if (equipo == 'A') {
            count_spawned_a++;
        } else if (equipo == 'B') {
            count_spawned_b++;
        }
        print_spawned(count_spawned_a, count_spawned_b);
    }
}

bool alguno_lemmings_max() {
    return sched_cantidad_lemmings('A') >= 5 || sched_cantidad_lemmings('B') >= 5;
}

void game_tick(void) {
    //breakpoint();
    //print_lemming_id(sched_devolver_lemming_actual()->id);
    if (var_game_over) {
        return;
    }
    ticks_passed++;
    sched_update_lemmings_vivos();
    if (ticks_passed % TICK_CREAR_LEMMING == 0 && alguno_menos_lemmings()) {
        crear_lemming_faltante();
    }
    if (ticks_passed % TICK_MATAR_LEMMING == 0 && alguno_lemmings_max()) {
        sched_reiniciar_lemming_mas_viejo();
    }
}

void game_init(void) {
    copiar_mapa_original();
    crear_lemming_faltante(); // crea lemming A
}
