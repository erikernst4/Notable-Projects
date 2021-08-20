#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "lib.h"



char hola[] = "hola";
char mundo[] = "mundo";
char empty = '\0';
int32_t diez = 10;
int32_t cinco = 5;
int32_t ocho = 8;   
char espada[] = "espada";
char copa[] = "copa";

void testInt(){
    int32_t mayor = intCmp(& diez, & cinco);
    int32_t menor = intCmp(& cinco, & diez);
    int32_t igual = intCmp(& cinco , & cinco);
    intPrint(&diez, stdout);
    intPrint(&cinco, stdout);   
    printf("mayor da %d \n", mayor);
    printf("menor da %d \n", menor);
    printf("igual da %d \n", igual);
}

void testString(){
    //strPrint(&(hola[0]), stdout);
    //strPrint(&(mundo[0]), stdout);
    //strPrint(&empty, stdout);

    char * holaclone = strClone(&hola[0]);
    strPrint(holaclone, stdout);
    strDelete(holaclone);

    uint32_t len = strLen(&mundo[0]); 
    printf("len de mundo es %d \n", len);


    int32_t res0 = strCmp(&mundo[0], &hola[0]);
    int32_t res1 = strCmp(&hola[0], &mundo[0]);
    int32_t res2 = strCmp(&mundo[0], &mundo[0]);
    int32_t res3 = strCmp(&empty, &mundo[0]);
    printf("mundo vs hola da %d \n", res0);
    printf("hola vs mundo da %d \n", res1);
    printf("mundo vs mundo da %d \n", res2);
    printf("empty vs mundo da %d \n", res3);
}

void testArray(){
    array_t* arreglo = arrayNew(1, 8);
    printf("Primero el tamaño del arreglo es %d \n", arrayGetSize(arreglo));
    arrayAddLast(arreglo, &diez);        
    int32_t* primerElemento_p = arrayGet(arreglo, 0);
    printf("se agregó %d \n", *(primerElemento_p));
    printf("Ahora el tamaño del arreglo es %d \n", arrayGetSize(arreglo));
    int32_t* borrado_p = arrayRemove(arreglo, 0);
    printf("Se borró el %d \n", *(borrado_p));
    free(borrado_p);    
    printf("Ahora el tamaño del arreglo es %d \n", arrayGetSize(arreglo));
    printf("El arreglo entero es \n");
    arrayPrint(arreglo, stdout);
    arrayDelete(arreglo); 
}

void testArraySwap(){
    array_t* arreglo = arrayNew(1, 3);
    printf("Primero el tamaño del arreglo es %d \n", arrayGetSize(arreglo));
    arrayAddLast(arreglo, &diez);
    arrayAddLast(arreglo, &diez);
    arrayAddLast(arreglo, &cinco);        
    int32_t* primerElemento_p = arrayGet(arreglo, 0);
    int32_t* segundoElemento_p = arrayGet(arreglo, 1);
    int32_t* tercerElemento_p = arrayGet(arreglo, 2);
    printf("Primer elemento: %d \n", *(primerElemento_p));
    printf("Segundo elemento: %d \n", *(segundoElemento_p));
    printf("Tercer elemento: %d \n", *(tercerElemento_p));
    printf("Ahora el tamaño del arreglo es %d \n", arrayGetSize(arreglo));
    arraySwap(arreglo, 0, 2);
    int32_t* nuevoPrimero_p = arrayGet(arreglo, 0);
    int32_t* nuevoSegundo_p = arrayGet(arreglo, 1);      
    printf("Se swapeo y ahora el primer elemento es %d \n", *(nuevoPrimero_p));
    printf("Y el segundo %d \n", *(nuevoSegundo_p));
    printf("El arreglo entero es: \n");
    arrayPrint(arreglo,stdout);
    arrayDelete(arreglo);    
}

void testList(){
    list_t* lista = listNew(1);
    uint8_t tam0 = listGetSize(lista);
    printf("el tamaño de la lista vac'ia es %d \n", tam0);
    printf("agregando 5 \n");
    listAddFirst(lista, &cinco);
    printf("agregando 10 \n");
    listAddFirst(lista, &diez);
    printf("agregando 5 \n");
    listAddFirst(lista, &cinco);
    uint8_t tam1 = listGetSize(lista);
    printf("el tamaño de la lista despues de agregar tres elementos es  %d \n", tam1);
    int32_t * elem = listGet(lista,1);
    printf("el segundo elemento es %d \n", *(elem));
    printf("agregando 10 \n");
    listAddFirst(lista, &diez);
    printf("agregando 5 \n");
    listAddFirst(lista, &cinco);
    //int32_t * removed1 = listRemove(lista,3);
    //printf("el primer elemento removido en la posicion 3 es %d \n", *removed1);
    //int32_t * removed2 = listRemove(lista,3);
    //printf("el segundo elemento removido en la posicion 3 es %d \n", *removed2);
    //free(removed1);
    //free(removed2);
    int32_t* elem1 = listGet(lista, 0);
    int32_t* elem2 = listGet(lista, 1);
    printf("el elemento [0] es %d\n", *elem1 );
    printf("el elemento [1] es %d\n", *elem2 );
    listSwap(lista,0,1);
    elem1 = listGet(lista,0);
    elem2 = listGet(lista,1);
    printf("el elemento [0] despues del swap es %d\n", *elem1 );
    printf("el elemento [1] despues del swap es %d\n", *elem2 );
    printf("La lista entera es \n");
    listPrint(lista, stdout);
    printf("agregando 10 atras \n");
    listAddLast(lista, &diez);
    printf("La lista entera es \n");
    listPrint(lista, stdout);
    listDelete(lista);
}
    
void testListSwap(){
    list_t* mazo = listNew(1);
    listAddFirst(mazo, &cinco);
    listAddFirst(mazo, &ocho);
    listAddFirst(mazo, &diez);
    listAddFirst(mazo, &diez);
    listAddFirst(mazo, &ocho);

    listPrint(mazo, stdout);

    listSwap(mazo,1,3);    
    listSwap(mazo,2,4);
    
    listPrint(mazo,stdout);
    
    listDelete(mazo);
}

void testCard(){
    card_t* carta1 = cardNew(&espada[0], &diez);
    card_t* carta2 = cardNew(&espada[0], &diez);
    card_t* carta3 = cardNew(&espada[0], &cinco);
    card_t* carta4 = cardNew(&copa[0], &diez);
    printf("Comparación mismas cartas: %d \n", cardCmp(carta1, carta2));
    printf("Comparación mismo palo distinto numero: %d \n", cardCmp(carta1, carta3));
    printf("Comparación distinto palo: %d \n", cardCmp(carta1, carta3));
    printf("Comparación distinto palo y distinto numero: %d \n", cardCmp(carta2, carta3));
    card_t* carta1copy = cardClone(carta1);
    card_t* carta3copy = cardClone(carta3);
    printf("Comparacion carta con su clon %d \n", cardCmp(carta1copy,carta1));
    printf("Comparacion de distinto palo (clones): %d\n", cardCmp(carta1copy, carta3copy));
    cardAddStacked(carta1, carta2);
    cardAddStacked(carta1, carta2);    
    cardAddStacked(carta4, carta1);

    printf("Carta1 es \n:");
    cardPrint(carta1, stdout);

    printf("Carta4 es \n:");
    cardPrint(carta4, stdout);


    cardDelete(carta1);
    cardDelete(carta2);
    cardDelete(carta3);
    cardDelete(carta4);
    cardDelete(carta1copy);
    cardDelete(carta3copy);
}


void testCardPrint(){
    

    card_t* carta1 = cardNew(&espada[0], &diez);
    card_t* carta2 = cardNew(&espada[0], &cinco);
    card_t* carta3 = cardNew(&copa[0], &diez);
    card_t* carta4 = cardNew(&copa[0], &cinco);
    
    cardAddStacked(carta1, carta2);
    cardAddStacked(carta1, carta3);    
    cardAddStacked(carta4, carta1);

    printf("Carta1 es \n:");
    cardPrint(carta1, stdout);

    printf("Carta4 es \n:");
    cardPrint(carta4, stdout);


    cardDelete(carta1);
    cardDelete(carta2);
    cardDelete(carta3);
    cardDelete(carta4);
}



void pruebaCorta_array(){
    card_t* carta1 = cardNew(&espada[0], &diez);
    card_t* carta2 = cardNew(&espada[0], &cinco);
    card_t* carta3 = cardNew(&copa[0], &diez);
    card_t* carta4 = cardNew(&copa[0], &cinco);
    card_t* carta5 = cardNew(&copa[0], &ocho);
   
    array_t* mazo = arrayNew(3, 5);
    arrayAddLast(mazo, carta1);
    arrayAddLast(mazo, carta2); 
    arrayAddLast(mazo, carta3); 
    arrayAddLast(mazo, carta4); 
    arrayAddLast(mazo, carta5);   

    arrayPrint(mazo, stdout);
    cardAddStacked(arrayGet(mazo, 0), arrayGet(mazo, 1));
    arrayPrint(mazo, stdout);
    arrayDelete(mazo);
    cardDelete(carta1);
    cardDelete(carta2);
    cardDelete(carta3);
    cardDelete(carta4);
    cardDelete(carta5);
}


void pruebaCorta_list(){
    card_t* carta1 = cardNew(&espada[0], &diez);
    card_t* carta2 = cardNew(&espada[0], &cinco);
    card_t* carta3 = cardNew(&copa[0], &diez);
    card_t* carta4 = cardNew(&copa[0], &cinco);
    card_t* carta5 = cardNew(&copa[0], &ocho);

    list_t* mazo = listNew(3);
    listAddFirst(mazo, carta5);
    listAddFirst(mazo, carta4);
    listAddFirst(mazo, carta3);
    listAddFirst(mazo, carta2);
    listAddFirst(mazo, carta1);

    listPrint(mazo, stdout);
    cardAddStacked(listGet(mazo, 0), listGet(mazo, 1));
    listPrint(mazo, stdout);
    listDelete(mazo);
    cardDelete(carta1);
    cardDelete(carta2);
    cardDelete(carta3);
    cardDelete(carta4);
    cardDelete(carta5);
}



    
int main (void){   

//    testInt();
//    testString();
//    testArray();
//    testArraySwap();
//    testList();
//    testListSwap();
//    testCard();
//    testCardPrint();
//    pruebaCorta_array();
//    pruebaCorta_list();

    return 0;
}



