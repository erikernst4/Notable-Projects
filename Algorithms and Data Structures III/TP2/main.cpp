#include <iostream>
#include <set>
#include <vector>
#include <tuple>
#include <queue>
#include <stack>
#include <chrono>
#include <climits>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;

int cantVertices;
int cantAristas;
vector<tuple<int, int, int>> aristas;
vector<vector<int>> costoArista;

const static int INDEFINIDO = INT_MAX;

// O(n)
int vecinoMasCercano(int vertice, const set<int> &verticesRestantes) {
    int min = INT_MAX;
    int argmin = -1;
    vector<int> costosDesdeVertice = costoArista[vertice];
    for (int otroVertice : verticesRestantes) {
        if (costosDesdeVertice[otroVertice] < min) {
            min = costosDesdeVertice[otroVertice];
            argmin = otroVertice;
        }
    }
    return argmin;
}

// O(n^2)
vector<int> heuristicaVecino() {
    int verticeAnterior = 0;
    vector<int> solucion{verticeAnterior};
    set<int> verticesRestantes;
    for (int i = 1; i < cantVertices; i++) {
        verticesRestantes.insert(i); // O(log n)
    } // O(n*log n)
    while (solucion.size() < cantVertices) {
        int vertice = vecinoMasCercano(verticeAnterior, verticesRestantes); // O(n)
        verticesRestantes.erase(vertice); // O(log n)
        solucion.push_back(vertice); // O(1)
        verticeAnterior = vertice;
    } // O(n^2)
    return solucion; // O(n)
}

// O(n*log n)
set<pair<int, int>> aristasDeSolucion(const vector<int> &solucion) {
    set<pair<int, int>> aristasCamino;
    int verticeAnterior = 0;
    for (int i = 1; i < solucion.size(); i++) {
        int v = solucion[i];
        aristasCamino.emplace(verticeAnterior, v); // O(log n)
        verticeAnterior = v;
    } // O(n*log n)
    // Cierro ciclo
    aristasCamino.emplace(verticeAnterior, 0); // O(log n)
    return aristasCamino; // O(n)
}

// O(n^2)
int elegirVertice(const vector<int> &solucion, set<int> &verticesRestantes) {
    int verticeMasCercano = -1;
    int minCosto = INT_MAX;
    for (int vertice : solucion) {
        for (int otroVertice : verticesRestantes) {
            if (costoArista[vertice][otroVertice] < minCosto) {
                minCosto = costoArista[vertice][otroVertice];
                verticeMasCercano = otroVertice;
            }
        } // O(n)
    } // O(n^2)
    return verticeMasCercano;
}

// O(n*log n)
vector<int> insertarVertice(int verticeAInsertar, const vector<int> &solucionParcial) {
    set<pair<int, int>> aristasSolucion = aristasDeSolucion(solucionParcial); // O(n*log n)
    int minCosto = INT_MAX;
    pair<int, int> insertarEntre = make_pair(-1, -1);
    for (pair<int, int> arista : aristasSolucion) {
        int costo = costoArista[arista.first][verticeAInsertar] + costoArista[verticeAInsertar][arista.second] -
                    costoArista[arista.first][arista.second];
        if (costo < minCosto) {
            minCosto = costo;
            insertarEntre = arista;
        }
    } // O(n)
    vector<int> nuevaSolucionParcial;
    for (int i = 0; i < solucionParcial.size(); i++) {
        int vertice = solucionParcial[i];
        nuevaSolucionParcial.push_back(vertice);
        if (i != solucionParcial.size() - 1) {
            int otroVertice = solucionParcial[i + 1];
            if (insertarEntre == make_pair(vertice, otroVertice)) {
                nuevaSolucionParcial.push_back(verticeAInsertar);
            }
        }
    } // O(n)
    if (nuevaSolucionParcial.size() == solucionParcial.size()) {
        nuevaSolucionParcial.push_back(verticeAInsertar);
    }
    return nuevaSolucionParcial; // O(n)
}

// O(n^3)
vector<int> heuristicaInsercion() {
    int verticeAzar1 = 0;
    int verticeAzar2 = 0;
    // Agarro 2 vertices no repetidos al azar
    while (verticeAzar1 == 0) {
        verticeAzar1 = rand() % cantVertices;
    }
    while (verticeAzar2 == 0 || verticeAzar2 == verticeAzar1) {
        verticeAzar2 = rand() % cantVertices;
    }
    // Los conecto entre si y con el primer vertice para formar un ciclo
    vector<int> solucion{0, verticeAzar1, verticeAzar2};
    set<int> verticesRestantes;
    for (int i = 1; i < cantVertices; i++) {
        if (i != verticeAzar1 && i != verticeAzar2) {
            verticesRestantes.insert(i); // O(log n)
        }
    } // O(n*log n)
    while (solucion.size() < cantVertices) {
        // ELEGIR mas cercano de alguno de los vertices del circuito
        int verticeMasCercano = elegirVertice(solucion, verticesRestantes); // O(n^2)
        // INSERTAR v tal que el circuito tenga el minimo costo posible
        solucion = insertarVertice(verticeMasCercano, solucion); // O(n*log n)
        verticesRestantes.erase(verticeMasCercano); // O(log n)
    } // O(n^3)
    return solucion; // O(n)
}

// O(n)
int calcularCosto(const vector<int> &solucion) {
    if (solucion.size() != cantVertices) {
        throw logic_error("Solucion no recorre todos los vertices una vez");
    }
    if (solucion[0] != 0) {
        throw logic_error("Solucion no empieza en el primer vertice");
    }
    int acumCosto = 0;
    int verticeAnterior = 0;
    for (int i = 1; i < solucion.size(); i++) {
        acumCosto += costoArista[verticeAnterior][solucion[i]];
        verticeAnterior = solucion[i];
    }
    // Cierro ciclo
    acumCosto += costoArista[verticeAnterior][0];
    return acumCosto;
}

// O(n)
vector<int> dosOptSwap(const vector<int> &candidato, int i, int j) {
    // Segun Wikipedia:
    // 1. take route[0] to route[i-1] and add them in order to new_route
    // 2. take route[i] to route[j] and add them in reverse order to new_route
    // 3. take route[j+1] to end and add them in order to new_route
    vector<int> nuevoCandidato = vector<int>(cantVertices); // O(n)
    // Step 1
    for (int k = 0; k < i; k++) {
        nuevoCandidato[k] = candidato[k];
    } // O(n)
    // Step 2
    for (int k = i; k <= j; k++) {
        nuevoCandidato[k] = candidato[j - k + i];
    } // O(n)
    // Step 3
    for (int k = j + 1; k < cantVertices; k++) {
        nuevoCandidato[k] = candidato[k];
    } // O(n)
    return nuevoCandidato; // O(n)
}

vector<vector<int>> filtrarRandomPorPorcentaje(vector<vector<int>> &vecindarioSoluciones, double porcentaje) {
    int nuevoSize = (int) round(vecindarioSoluciones.size() * porcentaje);
    auto rng = std::default_random_engine{std::random_device{}()};
    std::shuffle(std::begin(vecindarioSoluciones), std::end(vecindarioSoluciones), rng); // O(n^3)
    vector<vector<int>> vecindarioFiltrado(nuevoSize);
    std::copy_n(vecindarioSoluciones.begin(), nuevoSize, vecindarioFiltrado.begin()); // O(n^3)
    return vecindarioFiltrado; // O(n^3)
}

// O(n^3)
vector<vector<int>> dosOpt(vector<int> &mejorCandidato, double porcentaje) {
    vector<vector<int>> vecindarioSoluciones;
    for (int i = 1; i < cantVertices; i++) {
        for (int j = i + 1; j < cantVertices; j++) {
            vector<int> candidato = dosOptSwap(mejorCandidato, i, j); // O(n)
            vecindarioSoluciones.push_back(candidato); // O(n)
        } // O(n^2)
    } // O(n^3)
    // O(n^3), recordar que |vecindario| esta acotado sup por n^2
    return filtrarRandomPorPorcentaje(vecindarioSoluciones, porcentaje);
}

// O(n^3)
void encontrarMejorCandidato(vector<int> &mejorCandidato, int &costoMejorCandidato,
                             const vector<vector<int>> &vecindario) {
    mejorCandidato = vecindario[0]; // O(n)
    for (const vector<int> &candidato : vecindario) {
        int costoCandidato = calcularCosto(candidato); // O(n)
        if (costoCandidato < costoMejorCandidato) {
            mejorCandidato = candidato; // O(n)
            costoMejorCandidato = costoCandidato;
        }
    } // O(n^3), recordar que |vecindario| esta acotado sup por n^2
}

// O(n*log k)
void guardarSolucion(int k, const vector<int> &mejorCandidato, queue<set<vector<int>>::iterator> &colaUltimasKSols,
                     set<vector<int>> &setUltimasKSols) {
    auto iterSolucion = setUltimasKSols.insert(mejorCandidato).first; // O(n*log k)
    colaUltimasKSols.push(iterSolucion); // O(1)
    if (colaUltimasKSols.size() > k) {
        auto iterABorrar = colaUltimasKSols.front();
        colaUltimasKSols.pop(); // O(1)
        setUltimasKSols.erase(iterABorrar); // O(n*log k)
    }
}

// O((log k) * n^3)
vector<vector<int>> filtrarSolsRepetidas(const vector<vector<int>> &vecindarioSoluciones,
                                         const set<vector<int>> &setUltimasKSols) {
    vector<vector<int>> solucionesValidas;
    for (const vector<int> &solucion : vecindarioSoluciones) {
        if (setUltimasKSols.count(solucion) == 0) { // O(n*log k)
            solucionesValidas.push_back(solucion); // O(n)
        }
    } // O((log k) * n^3), recordar que |vecindario| esta acotado sup por n^2
    return solucionesValidas; // O(n^3)
}

// O(cantIters * n^3 * log k)
vector<int> tabuSoluciones(int k, int cantIteraciones, double porcentajeVecindad) {
    vector<int> mejorSolucion = heuristicaVecino(); // O(n^2)
    vector<int> mejorCandidato = mejorSolucion; // O(n)
    int costoMejorSolucion = calcularCosto(mejorSolucion); // O(n)
    int costoMejorCandidato = costoMejorSolucion;
    queue<set<vector<int>>::iterator> colaUltimasKSols;
    set<vector<int>> setUltimasKSols;
    auto iterSolucion = setUltimasKSols.insert(mejorCandidato).first; // O(n)
    colaUltimasKSols.push(iterSolucion);
    for (int i = 0; i < cantIteraciones; i++) {
        vector<vector<int>> vecindarioSols = dosOpt(mejorCandidato, porcentajeVecindad); // O(n^3)
        vecindarioSols = filtrarSolsRepetidas(vecindarioSols, setUltimasKSols); // O(logk * n^3)
        if (vecindarioSols.empty()) {
            auto iterABorrar = colaUltimasKSols.front();
            colaUltimasKSols.pop(); // O(1)
            setUltimasKSols.erase(iterABorrar); // O(n*log k)
            continue;
        }
        encontrarMejorCandidato(mejorCandidato, costoMejorCandidato, vecindarioSols); // O(n^3)
        if (costoMejorCandidato < costoMejorSolucion) {
            mejorSolucion = mejorCandidato; // O(n)
            costoMejorSolucion = costoMejorCandidato;
        }
        guardarSolucion(k, mejorCandidato, colaUltimasKSols, setUltimasKSols); // O(n*log k)
    } // O(cantIters * n^3 * log k)
    return mejorSolucion; // O(n)
}

// O(n*log n)
void guardarAristasModificadas(int k, const vector<int> &mejorCandidato, const vector<int> &mejorCandidatoPrevio,
                               queue<set<pair<int, int>>::iterator> &colaUltimasKAristas,
                               set<pair<int, int>> &setUltimasKAristas) {
    set<pair<int, int>> aristasPrevio = aristasDeSolucion(mejorCandidatoPrevio); // O(n*log n)
    set<pair<int, int>> aristasNuevo = aristasDeSolucion(mejorCandidato); // O(n*log n)
    for (pair<int, int> arista : aristasPrevio) {
        if (aristasNuevo.count(arista) == 0) { // O(log n)
            auto iterArista = setUltimasKAristas.insert(arista).first; // O(log k)
            colaUltimasKAristas.push(iterArista);
        }
    } // O(n*log n)
    while (colaUltimasKAristas.size() > k) {
        auto iterABorrar = colaUltimasKAristas.front();
        colaUltimasKAristas.pop();
        setUltimasKAristas.erase(iterABorrar); // O(log k)
    } // O(log k)
}

// O(k*log n)
bool esSolucionSinModAristas(const set<pair<int, int>> &aristasCamino,
                             const set<pair<int, int>> &setUltimasKAristas) {
    for (pair<int, int> arista : setUltimasKAristas) {
        // Las ultimas k aristas tienen que estar en el circuito
        pair<int, int> aristaAlReves = make_pair(arista.second, arista.first);
        if (!aristasCamino.count(arista) || !aristasCamino.count(aristaAlReves)) { // O(log n)
            return false;
        }
    } // O(k*log n)
    return true;
}

// O(log n * n^3)
vector<vector<int>> filtrarSolsConAristasModificadas(const vector<vector<int>> &vecindarioSols,
                                                     const set<pair<int, int>> &setUltimasKAristas) {
    vector<vector<int>> solucionesValidas;
    for (const vector<int> &solucion : vecindarioSols) {
        set<pair<int, int>> aristasCamino = aristasDeSolucion(solucion); // O(n*log n)
        if (esSolucionSinModAristas(aristasCamino, setUltimasKAristas)) { // O(k*log n)
            solucionesValidas.push_back(solucion); // O(n)
        }
    } // O(log n * n^3), |vecindarioSols| acotado sup por n^2
    return solucionesValidas; // O(n^3)
}

// O(cantIters * log n * n^3)
vector<int> tabuAristas(int k, int cantIteraciones, double porcentajeVecindario) {
    vector<int> mejorSolucion = heuristicaVecino(); // O(n^2)
    vector<int> mejorCandidato = mejorSolucion; // O(n)
    vector<int> mejorCandidatoPrevio;
    int costoMejorSolucion = calcularCosto(mejorSolucion); // O(n)
    int costoMejorCandidato = costoMejorSolucion;
    queue<set<pair<int, int>>::iterator> colaUltimasKAristas;
    set<pair<int, int>> setUltimasKAristas;
    for (int i = 0; i < cantIteraciones; i++) {
        vector<vector<int>> vecindarioSols = dosOpt(mejorCandidato, porcentajeVecindario); // O(n^3)
        mejorCandidatoPrevio = mejorCandidato; // O(n)
        vecindarioSols = filtrarSolsConAristasModificadas(vecindarioSols, setUltimasKAristas); // O(log n * n^3)
        if (vecindarioSols.empty()) {
            auto iterABorrar = colaUltimasKAristas.front();
            colaUltimasKAristas.pop(); // O(1)
            setUltimasKAristas.erase(iterABorrar); // O(n*log k)
            continue;
        }
        encontrarMejorCandidato(mejorCandidato, costoMejorCandidato, vecindarioSols); // O(n^3)
        if (costoMejorCandidato < costoMejorSolucion) {
            mejorSolucion = mejorCandidato; // O(n)
            costoMejorSolucion = costoMejorCandidato;
        }
        // logica guardado/borrado ultimas aristas
        guardarAristasModificadas(k, mejorCandidato, mejorCandidatoPrevio, colaUltimasKAristas,
                                  setUltimasKAristas); // O(n*log n)
    } // O(cantIters * log n * n^3)
    return mejorSolucion; // O(n)
}

// O(n)
void agregarAristaMinPrim(vector<int> &pred, vector<int> &pesoUnir, vector<bool> &visitado, vector<vector<int>> &adyacenciasAGM) {
    int min = INT_MAX;
    int minVertice;
    // Consigo el vertice que me cuesta menos agregar al arbol
    for (int i = 0; i < cantVertices; i++) {        // O(n)
        if (!visitado[i] && pesoUnir[i]<min) {
            min = pesoUnir[i];
            minVertice = i;
        }
    }
    adyacenciasAGM[pred[minVertice]].push_back(minVertice);
    adyacenciasAGM[minVertice].push_back(pred[minVertice]);
    visitado[minVertice] = true;
    // Actualizar predecesores
    for (int i = 0; i < cantVertices; i++) {    // O(n)
        if (i != minVertice) {
            if (!visitado[i] && costoArista[minVertice][i] < pesoUnir[i]) {
                pred[i] = minVertice;
                pesoUnir[i] = costoArista[minVertice][i];
            }
        }
    }
}

//O(n^2)
vector<vector<int>> prim() {
    vector<vector<int>> adyacenciasAGM(cantVertices);       // O(n)
    vector<bool> visitado(cantVertices, false);       // O(n)
    vector<int> pesoUnir(cantVertices, INT_MAX);            // pesoUnir[i] guarda el coste minimo de agregar al AGM el vertice i
    vector<int> pred(cantVertices);                         // pred[i] es el predecesor del vertice i en el AGM
    // Inicializo el primer vertice
    visitado[0]=true;
    pesoUnir[0]=0;
    for (int i = 1; i < cantVertices; i++) {                // O(n)
        pesoUnir[i]=costoArista[0][i];
        pred[i]=0;
    }
    // Construyo el vector de adyacencias del AGM
    for (int i = 1; i < cantVertices; i++) {                // O(n^2)
        agregarAristaMinPrim(pred,pesoUnir,visitado,adyacenciasAGM);    // O(n)
    }
    return adyacenciasAGM;  // O(n)
}

// O(n + m)
vector<int> recorridoDFS(vector<vector<int>> &adyacencias) {
    vector<int> recorrido(cantVertices);                      // O(n)
    vector<bool> visitados(cantVertices,false);         // O(n)
    stack<int> stackDFS;
    int nodoActual;
    int nodoSiguiente;
    // Vertice 0 es la raiz
    visitados[0] = true;
    int next = 0; // Va indicando el orden de los vertices
    recorrido[next] = 0;
    stackDFS.push(0);
    while (!stackDFS.empty()) {         //  O(n + m)
        nodoActual = stackDFS.top();
        nodoSiguiente = -1;
        for (int i = 0; i < adyacencias[nodoActual].size(); i++) {
            if (!visitados[adyacencias[nodoActual][i]]){
                nodoSiguiente = adyacencias[nodoActual][i];
            }
        }
        if (nodoSiguiente == -1){
            stackDFS.pop();
        } else {
            visitados[nodoSiguiente] = true;
            next++;
            recorrido[next] = nodoSiguiente;
            stackDFS.push(nodoSiguiente);
        }
    }
    return recorrido;   // O(n)
}

// O(n^2)
vector<int> heuristicaAGM() {
    // Obtener AGM
    vector<vector<int>> adyacenciasAGM = prim();        // O(n^2)
    // Realizar DFS desde 0
    vector<int> res = recorridoDFS(adyacenciasAGM); // O(n_agm + m_agm) = O(n + m) = O(n + n^2) = O(n^2)
    return  res; // O(n)
}

void inicializarYValidar() {
    cin >> cantVertices >> cantAristas;
    costoArista = vector<vector<int>>(cantVertices, vector<int>(cantVertices, INDEFINIDO));
    for (int i = 0; i < cantAristas; i++) {
        int v;
        int w;
        int c;
        cin >> v >> w >> c;
        aristas.emplace_back(v - 1, w - 1, c);
        costoArista[v - 1][w - 1] = c;
        costoArista[w - 1][v - 1] = c;
    }
    for (int i = 0; i < cantVertices; i++) {
        for (int j = 0; j < cantVertices; j++) {
            if (i != j && costoArista[i][j] == INDEFINIDO) {
                throw logic_error("Arista no definida, no es completo");
            }
        }
    }
    if (cantVertices < 3) {
        throw logic_error("Grafo tiene que tener por lo menos tres vertices para ser hamiltoniano");
    }
    if (cantAristas != cantVertices * (cantVertices - 1) / 2) {
        throw logic_error("Grafo no tiene n(n-1)/2 aristas (no es completo)");
    }
}

void imprimirResultado(vector<int> &solucion) {
    // Por el ejemplo que dan en el enunciado no hay que considerar el costo de cerrar el ciclo
    // int costoCerrarCiclo = costoArista[solucion[solucion.size() - 1]][0];
    int minCosto = calcularCosto(solucion); //- costoCerrarCiclo;
    cout << cantVertices << ' ' << minCosto << endl;
    string solucionOutput;
    for (int i = 0; i < solucion.size(); i++) {
        if (i != 0) {
            solucionOutput += ' ';
        }
        solucionOutput += to_string(solucion[i] + 1);
    }
    cout << solucionOutput << endl;
}

bool empiezaCon(string &str, const string &substr) {
    return str.rfind(substr, 0) == 0;
}

int main(int argc, char **argv) {
    inicializarYValidar();
    auto comienzo = chrono::steady_clock::now();
    vector<int> result;
    string algoritmo = argv[1];
    //string algoritmo = "AGM";
    if (algoritmo == "VEC") {
        result = heuristicaVecino();
    } else if (algoritmo == "INS") {
        result = heuristicaInsercion();
    } else if (algoritmo == "AGM") {
        result = heuristicaAGM();
    } else if (empiezaCon(algoritmo, "TABU")) {
        int k = stoi(argv[2]);
        int cantIteraciones = stoi(argv[3]);
        double porcentajeVecindad = stod(argv[4]); // Rango 0-1
        if (porcentajeVecindad > 1 || porcentajeVecindad < 0) {
            throw logic_error("Porcentaje invalido, tiene que ser del 0 al 1 inclusive");
        }
        if (algoritmo == "TABU_S") {
            result = tabuSoluciones(k, cantIteraciones, porcentajeVecindad);
        } else if (algoritmo == "TABU_A") {
            result = tabuAristas(k, cantIteraciones, porcentajeVecindad);
        } else {
            throw logic_error("Algoritmo invalido");
        }
    } else {
        throw logic_error("Algoritmo invalido");
    }
    auto fin = chrono::steady_clock::now();
    double tiempoTotal = chrono::duration<double, milli>(fin - comienzo).count();
    clog << tiempoTotal << endl;
    imprimirResultado(result);
    return 0;
}
