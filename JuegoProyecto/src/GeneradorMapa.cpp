#include "GeneradorMapa.h"
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

using namespace GeneradorMapa;

static std::mt19937 &rng()
{
  static std::mt19937 gen(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  return gen;
}

static int pesoLog(float d)
{
  std::uniform_int_distribution<int> ruido(-1, 1);
  int peso = (int)std::log1p(d) + ruido(rng());
  peso = std::max(1, peso);
  return peso;
}

static float dist(float x1, float y1, float x2, float y2)
{
  return std::hypot(x1 - x2, y1 - y2);
}

void GeneradorMapa::generarMapa(
    std::vector<NodoInfo> &nodos,
    std::vector<AristaInfo> &aristas,
    const Config &cfg)
{
  nodos.clear();
  aristas.clear();

  int N = cfg.totalNodos;
  int R = std::min(cfg.cantRecursos, N - 1);

  float minSpacing = cfg.radioNodo * cfg.multiplicadorDistancia;

  std::uniform_real_distribution<float> distX(50.f, cfg.ancho - 50.f);
  std::uniform_real_distribution<float> distY(50.f, cfg.alto - 50.f);
  std::uniform_int_distribution<int> distPeso(cfg.pesoMin, cfg.pesoMax);
  std::uniform_int_distribution<int> distValor(200, 1200);
  std::uniform_real_distribution<float> prob(0.f, 1.f);

  // =========================================================================
  // 1) Generar nodos con separación mínima
  // Si la distancia a cualquier nodo existente es < minSpacing -> lo rechaza
  for (int i = 0; i < N; i++)
  {
    bool colocado = false;
    /// @warning Si ocurren mas de 2000 intentos se deja en cualquier sitio, sin
    /// comprobar spacing
    for (int intento = 0; intento < 2000 && !colocado; intento++)
    {
      float x = distX(rng());
      float y = distY(rng());
      bool ok = true;

      for (auto &n : nodos)
        if (dist(x, y, n.x, n.y) < minSpacing)
        {
          ok = false;
          break;
        }

      if (ok)
      {
        // Si encuentra posición válida crea NodoInfo
        NodoInfo ni;
        ni.id = i;
        ni.x = x;
        ni.y = y;
        ni.tipo = 0;
        nodos.push_back(ni);
        colocado = true;
      }
    }

    if (!colocado)
    {
      NodoInfo ni;
      ni.id = i;
      ni.x = distX(rng());
      ni.y = distY(rng());
      nodos.push_back(ni);
    }
  }

  nodos[0].tipo = 1;
  for (int i = 1; i <= R; i++)
  {
    nodos[i].tipo = 2;
    nodos[i].valor = distValor(rng());
  }

  // =========================================================================
  // 2) Construir un MST con límite de grado
  std::vector<bool> usado(N, false);
  std::vector<int> degree(N, 0);
  usado[0] = true;

  auto distGeo = [&](int a, int b)
  {
    return dist(nodos[a].x, nodos[a].y, nodos[b].x, nodos[b].y);
  };
  // Se busca la arista u → v más corta tal que:
  // u ya está en el árbol (usado[u] == true)
  // v aún no (usado[v] == false)
  // degree[u] < gradoMaximo y degree[v] < gradoMaximo
  for (int rep = 0; rep < N - 1; rep++)
  {
    float mejor = 1e9;
    int mu = -1, mv = -1;

    for (int u = 0; u < N; u++)
    {
      if (!usado[u])
        continue;
      if (degree[u] >= cfg.gradoMaximo)
        continue;

      for (int v = 0; v < N; v++)
      {
        if (usado[v])
          continue;
        if (degree[v] >= cfg.gradoMaximo)
          continue;

        float d = dist(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);
        if (d < mejor)
        {
          mejor = d;
          mu = u;
          mv = v;
        }
      }
    }

    if (mu != -1 && mv != -1)
    {
      usado[mv] = true;
      degree[mu]++;
      degree[mv]++;
      float d = distGeo(mu, mv);
      AristaInfo a;
      a.idOrigen = mu;
      a.idDestino = mv;
      a.peso = pesoLog(d);
      aristas.push_back(a);
    }
  }

  // =========================================================================
  // 3) Aristas extra con límite de grado
  for (int i = 0; i < N; i++)
  {
    for (int j = i + 1; j < N; j++)
    {
      if (prob(rng()) < cfg.probExtraEdges) // Probabilidad de arista extra
      {
        if (degree[i] >= cfg.gradoMaximo)
          continue;
        if (degree[j] >= cfg.gradoMaximo)
          continue;

        AristaInfo a;
        a.idOrigen = i;
        a.idDestino = j;
        a.peso = distPeso(rng());
        aristas.push_back(a);

        degree[i]++;
        degree[j]++;
      }
    }
  }
}
