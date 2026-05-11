#ifndef ALGORITMOS_GRAFO_H
#define ALGORITMOS_GRAFO_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include "Node.h"

/**
 * @brief Encuentra el camino más corto desde 'origen' hasta 'destino' usando BFS.
 *s
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @return std::vector<int> Camino desde origen hasta destino (lista de nodos).
 */
std::vector<int> ruta_bfs(const std::unordered_map<int, Node> &grafo,
                          int origen, int destino);

/**
 * @brief Encuentra un camino desde 'origen' hasta 'destino' usando DFS.
 *
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @return std::vector<int> Camino desde origen hasta destino (lista de nodos).
 */
std::vector<int> ruta_dfs(const std::unordered_map<int, Node> &grafo,
                          int origen, int destino);

/**
 * @brief Calcula el peso total de un camino dado en el grafo.
 *
 * @param path Vector de nodos que representa el camino.
 * @param grafo Grafo representado como un mapa de nodos.
 * @return int Suma de los pesos de las aristas en el camino.
 */
int calcWeights(std::vector<int> path,
                const std::unordered_map<int, Node> &grafo);

/**
 * @brief Encuentra el camino mínimo desde 'origen' hasta 'destino' usando el algoritmo de Prim.
 *
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @return std::vector<int> Camino mínimo según Prim.
 */
std::vector<int> prim_camino(
    const std::unordered_map<int, Node> &grafo, int origen, int destino);

/**
 * @brief Encuentra el camino más corto desde 'origen' hasta 'destino' usando A*.
 *
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @return std::vector<int> Camino desde origen hasta destino (lista de nodos).
 */
std::vector<int> ruta_astar(const std::unordered_map<int, Node> &grafo,
                            int origen, int destino);

/**
 * @brief Calcula las distancias mínimas desde 'origen' a todos los nodos del grafo usando Dijkstra.
 *        También llena el mapa de predecesores para reconstruir caminos.
 *
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param distancias Mapa donde se almacenarán las distancias mínimas desde el origen.
 * @param predecesores Mapa donde se almacenarán los predecesores de cada nodo.
 */
void dijkstra_con_camino(const std::unordered_map<int, Node> &grafo,
                         int origen,
                         std::unordered_map<int, int> &distancias,
                         std::unordered_map<int, int> &predecesores);

/**
 * @brief Reconstruye el camino óptimo desde 'origen' hasta 'destino' usando un mapa de predecesores.
 *
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @param predecesores Mapa de predecesores generado por Dijkstra.
 * @return std::vector<int> Camino reconstruido desde origen hasta destino.
 */
std::vector<int> reconstruir_camino_dijkstra(
    int origen,
    int destino,
    const std::unordered_map<int, int> &predecesores);

/**
 * @brief Encuentra el camino más corto desde 'origen' hasta 'destino' usando Dijkstra.
 *
 * @param grafo Grafo representado como un mapa de nodos.
 * @param origen Nodo de inicio.
 * @param destino Nodo destino.
 * @return std::vector<int> Camino desde origen hasta destino (lista de nodos).
 */
std::vector<int> ruta_dijkstra(const std::unordered_map<int, Node> &grafo,
                               int origen, int destino);

#endif // ALGORITMOS_GRAFO_H
