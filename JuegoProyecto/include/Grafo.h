#ifndef GRAFO_H
#define GRAFO_H

#include <unordered_map>
#include <vector>
#include "Node.h"
#include "LectorArchivos.h"

/**
 * @brief Representa un grafo mediante lista de adyacencia.
 *
 * Cada nodo se almacena en un mapa que relaciona su ID con un objeto Node,
 * que contiene información de sus vecinos y aristas.
 */
class Grafo
{
private:
  /// Lista de adyacencia: cada nodo con su información y vecinos
  std::unordered_map<int, Node> nodos;

public:
  /// Constructor por defecto
  Grafo() = default;

  /**
   * @brief Construye el grafo a partir de vectores de nodos y aristas.
   *
   * @param nodosInfo Vector con información de los nodos.
   * @param aristas Vector con información de las aristas.
   *
   * Esta función llena la lista de adyacencia del grafo usando
   * los datos proporcionados.
   */
  void construir(const std::vector<NodoInfo> &nodosInfo,
                 const std::vector<AristaInfo> &aristas);

  /**
   * @brief Obtiene los nodos del grafo.
   *
   * @return const std::unordered_map<int, Node>& Referencia constante a los nodos.
   */
  const std::unordered_map<int, Node> &obtenerNodos() const
  {
    return nodos;
  }
};

#endif // GRAFO_H
