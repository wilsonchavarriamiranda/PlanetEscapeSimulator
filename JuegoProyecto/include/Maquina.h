#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Representa una máquina instalada en un nodo del grafo.
 *
 * Cada máquina tiene un nivel, un tipo y está asociada a un nodo específico.
 */
class Maquina
{
private:
  /// Nivel de la máquina (de 1 a 4)
  int nivel;

  /// Tipo de máquina (1: BFS, 2: DFS)
  int tipo;

  /// ID del nodo donde está instalada
  int nodo;

public:
  /// Constructor por defecto (nivel 0, tipo 0, nodo -1)
  Maquina() : nivel(0), tipo(0), nodo(-1) {}

  /**
   * @brief Constructor con parámetros.
   *
   * @param nodoID ID del nodo donde se instala la máquina.
   * @param tipoInicial Tipo de máquina (1: BFS, 2: DFS).
   * @param nivelInicial Nivel inicial de la máquina (por defecto 1).
   */
  Maquina(int nodoID, int tipoInicial, int nivelInicial = 1)
  {
    nodo = nodoID;
    tipo = tipoInicial;
    nivel = nivelInicial;
  }

  /**
   * @brief Incrementa el nivel de la máquina en 1.
   */
  void sumNivel() { nivel++; }

  /**
   * @brief Mejora la máquina (implementación específica del juego).
   */
  void mejorar();

  /**
   * @brief Obtiene el nivel actual de la máquina.
   * @return int Nivel de la máquina.
   */
  int getNivel() const { return nivel; }

  /**
   * @brief Obtiene el nodo donde está instalada la máquina.
   * @return int ID del nodo.
   */
  int getNodo() const { return nodo; }

  /**
   * @brief Dibuja la máquina en la ventana de SFML.
   *
   * @param window Referencia a la ventana SFML donde se dibujará.
   * @param pos Posición donde se dibuja la máquina.
   * @param radioNodo Radio del nodo (para escalar la máquina correctamente).
   */
  void draw(sf::RenderWindow &window, sf::Vector2f pos, float radioNodo);
};
