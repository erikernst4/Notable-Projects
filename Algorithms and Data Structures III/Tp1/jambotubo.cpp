#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;

int INFTY = 10e6; // Valor para indicar que no hubo solución.

// Información de la instancia a resolver.
/*
int n;
int R;
vector<int> w;
vector<int> r;
 */
int n;
int R ;
vector<int> w;
vector<int> r;

// Funciones Auxiliares

// De los elementos que quedan de i en adelante, devuelve el indice del que tiene el menor peso
int min_w (int i){
    int min = INFTY;
    for (int j = i; j < n; ++j) {
        if (w[j] < min) min = w[j];
    }
    return min;
}

// De los elementos que quedan de i en adelante, devuelve cuantos son menores al r_actual
// Es decir, de los elementos que quedan, indica cuantos teoricamente (y no necesariamente juntos) pueden entrar en la bolsa en este estado
int w_viables(int i, int r_actual){
    int viables = 0;
    for (int j = i; j < n; ++j) {
        if (w[j] <= r_actual) viables++;
    }
    return viables;
}


// p: cantidad de elementos seleccionados en este nodo.
// i: posicion del elemento a considerar en este nodo.
// W: es el peso del jambotubo en este nodo.
// r_actual: es el maximo peso soportado en este nodo.
int FB(int p, int i, int W, int r_actual, bool productoAplastado){
    // caso base
    if (i == n) {
        if(W > R || productoAplastado) return 0;
        return p;
    }

    //recursion
    int no_agrego = FB(p, i + 1, W, r_actual, productoAplastado);
    // analizamos si este elemento aplasta algún otro.
    if (w[i] > r_actual) {
        productoAplastado = true;
    }
    int agrego = FB(p + 1, i + 1, W + w[i], min(r_actual - w[i], r[i]), productoAplastado);
    return max(agrego, no_agrego);
}

// p: cantidad de elementos seleccionados en este nodo.
// i: posicion del elemento a considerar en este nodo.
// W: es el peso del jambotubo en este nodo.
// r_actual: es el maximo peso soportado en este nodo.
bool poda_factibilidad1 = true; // define si la poda por FACTIBILIDAD con complejidad de orden CONSTANTE está habilitada.
bool poda_factibilidad2 = true; // define si la poda por FACTIBILIDAD con complejidad de orden LINEAL está habilitada.
bool poda_optimalidad1 = true; // define si la poda por OPTIMALIDAD con complejidad de orden CONSTANTE está habilitada.
bool poda_optimalidad2 = true; // define si la poda por OPTIMALIDAD con complejidad de orden LINEAL está habilitada.
int K = 0; // Mejor solucion hasta el momento.
int BT(int p, int i, int W, int r_actual, bool productoAplastado) {
    // Caso base.
    if (i == n) {
        if(W > R) return 0; //solucion no valida. (no deberia dar true nunca ya que evitamos excedernos del peso)
        if (p > K) K = p;
        return p;
    }

    // Poda por factibilidad 1.
    // si el último elemento agregado generó aplastamiento o se superó la resistencia del Jambotubo.
    if (poda_factibilidad1 && (productoAplastado || W > R)) return 0;

    // Poda por factibilidad 2.
    //si el minimo peso de los elementos que quedan supera el r_actual, podamos.
    if (poda_factibilidad2 && min_w(i) > r_actual){
        if (p > K) K = p;
        return p;
    }

    // Poda por optimalidad 1.
    // Si se agregan todos los elementos restantes y no se supera el K, podamos.
    if (poda_optimalidad1 && (n - i + p <= K)) return p;

    // Poda por optimalidad 2.
    // Si la cantidad de elementos restantes con peso apto (<= r_actual) no hacen superar K, podamos.
    if (poda_optimalidad2 && (w_viables(i, r_actual) + p <= K)) return p;

    // Recursión.
    int no_agrego = BT(p, i + 1, W, r_actual, productoAplastado);
    // Si agregar el elemento quiebra alguna resistencia, no lo agregamos.
    if (w[i] > r_actual) {
        productoAplastado = true;
    }
    int agrego = BT(p+1, i + 1, W + w[i], min(r_actual - w[i], r[i]), productoAplastado);
    return max(no_agrego, agrego);
}

const int indefinido = -1;
vector<vector<int>> M; // Memoria de PD.


int PD(int i, int r_actual){
    if(M[i][r_actual] == indefinido){
        if (i == n) M[i][r_actual] = 0;
        else if (w[i] > r_actual) M[i][r_actual] = PD(i + 1, r_actual);
        else M[i][r_actual] = max(PD(i + 1, r_actual), 1 + PD(i + 1, min(r_actual - w[i], r[i])));
    }
    return M[i][r_actual];
}

// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.

int main(int argc, char* argv[]){

    //for (int i = 0; i < argc; ++i) {
    //     cout << argv[i] << endl;
    //}

    string algoritmo = argv[1];
    cin >> n >> R;
    for(int i=0; i< n; i++) {
        int w_i;
        int r_i;
        cin >> w_i >> r_i;
        w.push_back(w_i);
        r.push_back(r_i);
    }

    //cout << n << R << endl;
    //for(int i=0; i< n; i++) {
    //    cout << w[i] << r[i] << endl;
    //}


    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum = 0;
    //optimum = INFTY;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB")
    {
        optimum = FB(0,0,0, R, false);
    }
    else if (algoritmo == "BT")
    {
        K = 0;
        poda_factibilidad1 = poda_factibilidad2 = true;
        poda_optimalidad1 = poda_optimalidad2 = true;

        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-F")
    {
        K = 0;
        poda_factibilidad1 = poda_factibilidad2 = true;
        poda_optimalidad1 = poda_optimalidad2 = false;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-F1") {
        K = 0;
        poda_factibilidad1 = true;
        poda_optimalidad1 = poda_optimalidad2 = poda_factibilidad2 = false;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-F2") {
        K = 0;
        poda_factibilidad2 = true;
        poda_optimalidad1 = poda_optimalidad2 = poda_factibilidad1 = false;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-O")
    {
        K = 0;
        poda_factibilidad1 = poda_factibilidad2 = false;
        poda_optimalidad1 = poda_optimalidad2 = true;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-O1") {
        K = 0;
        poda_optimalidad1 = true;
        poda_factibilidad1 = poda_optimalidad2 = poda_factibilidad2 = false;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "BT-O2") {
        K = 0;
        poda_optimalidad2 = true;
        poda_factibilidad1 = poda_optimalidad1 = poda_factibilidad2 = false;
        optimum = BT(0, 0, 0, R, false);
    }
    else if (algoritmo == "PD")
    {
        M = vector<vector<int>> (n + 2, vector<int> (R + 1, indefinido));

        // Obtenemos la solucion optima.
        optimum = PD(0,R);
    }
    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << /*(optimum == INFTY ? -1 : optimum)*/ optimum << endl;

    return 0;
}
