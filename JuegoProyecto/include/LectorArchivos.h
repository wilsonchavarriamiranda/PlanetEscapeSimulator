#ifndef LECTORARCHIVOS_H
#define LECTORARCHIVOS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

/**
 * @brief Representa la información básica de un nodo.
 */
struct NodoInfo
{
  /// Identificador único del nodo
  int id;

  /// Tipo de nodo (puede usarse para diferenciar recursos, obstáculos, etc.)
  int tipo;

  /// Valor asociado al nodo (por ejemplo, cantidad de recurso)
  int valor;

  /// Coordenada X del nodo en el mapa
  int x;

  /// Coordenada Y del nodo en el mapa
  int y;
};

/**
 * @brief Representa la información básica de una arista.
 */
struct AristaInfo
{
  /// ID del nodo origen
  int idOrigen;

  /// ID del nodo destino
  int idDestino;

  /// Peso de la arista (costo de la conexión)
  int peso;
};

/**
 * @brief Lee nodos y aristas desde un archivo.
 *
 * @param nombreArchivo Nombre del archivo a leer.
 * @param nodos Vector donde se almacenarán los nodos leídos.
 * @param aristas Vector donde se almacenarán las aristas leídas.
 * @return true Si la lectura fue exitosa.
 * @return false Si ocurrió algún error al abrir o leer el archivo.
 *
 * Esta función parsea el contenido del archivo y llena los vectores
 * de nodos y aristas para su posterior uso en la construcción del grafo.
 */
bool leerNodos(
    const std::string &nombreArchivo,
    std::vector<NodoInfo> &nodos,
    std::vector<AristaInfo> &aristas);

#endif // LECTORARCHIVOS_H
