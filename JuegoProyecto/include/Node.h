#ifndef NODE_H
#define NODE_H

#include <vector>
#include <utility>

/**
 * @brief Representa un nodo en un grafo.
 *
 * Contiene información básica del nodo, su posición y sus vecinos.
 */
struct Node
{
  /// Identificador único del nodo
  int id;

  /// Tipo de nodo (puede representar recurso, obstáculo, etc.)
  int tipo;

  /// Valor asociado al nodo (por ejemplo, cantidad de recurso)
  int valor;

  /// Coordenada X del nodo
  int x;

  /// Coordenada Y del nodo
  int y;

  /// Lista de vecinos: cada par contiene (ID del nodo vecino, peso de la arista)
  std::vector<std::pair<int, int>> neighbors;
};

#endif // NODE_H
