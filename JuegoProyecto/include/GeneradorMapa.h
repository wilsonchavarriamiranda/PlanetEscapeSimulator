#pragma once
#include <vector>
#include "LectorArchivos.h"

namespace GeneradorMapa
{
  /**
   * @brief Configuración para la generación de mapas.
   *
   * Contiene parámetros que determinan la cantidad de nodos,
   * recursos, dimensiones del mapa, pesos de aristas y probabilidades
   * de conexiones extra.
   */
  struct Config
  {
    /// Número total de nodos a generar
    int totalNodos = 10;

    /// Cantidad de recursos en el mapa (aprox. un tercio de los nodos)
    int cantRecursos = (int)totalNodos / 3;

    /// Dimensiones del mapa (ancho)
    float ancho = 1900.f;

    /// Dimensiones del mapa (alto)
    float alto = 1100.f;

    /// Radio de los nodos para calcular espaciamiento
    float radioNodo = 15.f;

    /// Multiplicador de distancia para evitar colisiones
    float multiplicadorDistancia = 4.0f;

    /// Peso mínimo de las aristas
    int pesoMin = 2;

    /// Peso máximo de las aristas
    int pesoMax = 20;

    /// Probabilidad de agregar aristas adicionales
    float probExtraEdges = 0.12f;

    /// Grado máximo permitido por nodo
    int gradoMaximo = 3;
  };

  /**
   * @brief Genera un mapa aleatorio de nodos y aristas.
   *
   * @param nodos Vector donde se almacenarán los nodos generados.
   * @param aristas Vector donde se almacenarán las aristas generadas.
   * @param cfg Configuración opcional para la generación del mapa.
   */
  void generarMapa(
      std::vector<NodoInfo> &nodos,
      std::vector<AristaInfo> &aristas,
      const Config &cfg = Config());
}
