#include "AlgoritmosGrafo.h"
#include <limits>

std::vector<int> ruta_bfs(const std::unordered_map<int, Node> &grafo, int origen, int destino)
{
  if (origen == destino)
    return {origen}; /// Caso base
  if (grafo.count(origen) == 0)
    return {}; /// Nodo origen no existe

  std::queue<int> q;                   /// FIFO
  std::unordered_set<int> visited;     /// Conjunto de nodos visitados(irrepetibles)
  std::unordered_map<int, int> parent; /// Trazabilidad del camino

  q.push(origen);
  visited.insert(origen);
  parent[origen] = -1; /// Clave=origen con valor -1 para determinar camino

  bool found = false;
  /// Quedan nodos por visitar Y no se ha encontrado el destino
  while (!q.empty() && !found)
  {

    int nodoActual = q.front(); /// FIFO para asegurar la busqueda por amplitud
    q.pop();

    /// Si no existe como clave en el mapa, no tiene vecinos en si.
    if (grafo.count(nodoActual) == 0)
      continue; /// Saltamos al siguiente en cola

    /// referencia al 'nodoActual' del mapa
    const Node &node = grafo.at(nodoActual);

    /// Buscar por amplitud en todos los vecinos del 'nodoActual'
    for (size_t i = 0; i < node.neighbors.size(); ++i)
    {
      int v = node.neighbors[i].first;
      /// Si no visitado, lo visitamos y guardamos su padre
      if (visited.find(v) == visited.end())
      { // returns end() cuando no existe
        visited.insert(v);
        /// Clave: vecino-->'v', Valor: padre-->'nodoActual'
        parent[v] = nodoActual; /// guardamos traza del anterior al vecino
        if (v == destino)
        {
          found = true;
          break;
        }
        q.push(v); /// Si no es destino, agregar a cola para visitarlo luego
      } /// ya fue visitado, no hacemos nada y continuamos...
    }
  }

  if (!found)
    return {};

  /// Reconstruir camino usando mapa desordenado --> parent
  std::vector<int> path;
  int currentNode = destino;
  while (currentNode != -1)
  {
    /* Si se usa una alternativa similar a push_front() eliminamos el costo de
    hacer reverse pero perdemos rendimiento porque cada vez que se inserta al
    frente del vector se corren todos los elementos una posicion a la derecha
    esto cuesta O(n) cada vez, y si se reconstruye un camino de longitud n
    provoca O(n²) en total */
    path.push_back(currentNode);                    /// Reconstruir desde destino
    auto parentIterator = parent.find(currentNode); /// Buscar pair con clave
    currentNode = parentIterator->second;           /// padre/nodo desde el que se accedió
  }
  std::reverse(path.begin(), path.end());
  if (path.empty() || path.front() != origen)
    return {};
  return path;
}

std::vector<int> ruta_dfs(const std::unordered_map<int, Node> &grafo, int origen, int destino)
{
  if (origen == destino)
    return {origen}; /// Caso base
  if (grafo.count(origen) == 0)
    return {}; /// Nodo origen no existe

  std::stack<int> st;                  /// LIFO
  std::unordered_set<int> visited;     /// Conjunto de nodos visitados(irrepetibles)
  std::unordered_map<int, int> parent; /// Trazabilidad del camino

  st.push(origen);
  visited.insert(origen);
  parent[origen] = -1; /// Clave=origen con valor -1 para determinar camino

  bool found = false;
  /// Quedan nodos por visitar Y no se ha encontrado el destino
  while (!st.empty() && !found)
  {

    int nodoActual = st.top(); /// LIFO para asegurar la busqueda por produndidad
    st.pop();

    /// Si no existe como clave en el mapa, no tiene vecinos en si.
    if (grafo.count(nodoActual) == 0)
      continue; /// Saltamos al siguiente en cola

    /// referencia al 'nodoActual' del mapa
    const Node &node = grafo.at(nodoActual);

    /// Buscar por profundidad en todos los vecinos del 'nodoActual'
    for (size_t i = 0; i < node.neighbors.size(); ++i)
    {
      int v = node.neighbors[i].first;
      /// Si no visitado, lo visitamos y guardamos su padre
      if (visited.find(v) == visited.end())
      { // returns end() cuando no existe
        visited.insert(v);
        /// Clave: vecino-->'v', Valor: padre-->'nodoActual'
        parent[v] = nodoActual; /// guardamos traza del anterior al vecino
        if (v == destino)
        {
          found = true;
          break;
        }
        st.push(v); /// Si no es destino, agregar al tope para buscarlo en profundidad
      } /// ya fue visitado, no hacemos nada y continuamos...
    }
  }

  if (!found)
    return {};

  /// Reconstruir camino usando mapa desordenado --> parent
  std::vector<int> path;
  int currentNode = destino;
  while (currentNode != -1)
  {
    /* Si se usa una alternativa similar a push_front() eliminamos el costo de
    hacer reverse pero perdemos rendimiento porque cada vez que se inserta al
    frente del vector se corren todos los elementos una posicion a la derecha
    esto cuesta O(n) cada vez, y si se reconstruye un camino de longitud n
    provoca O(n²) en total */
    path.push_back(currentNode);                    /// Reconstruir desde destino
    auto parentIterator = parent.find(currentNode); /// Buscar pair con clave
    currentNode = parentIterator->second;           /// padre/nodo desde el que se accedió
  }
  std::reverse(path.begin(), path.end());
  if (path.empty() || path.front() != origen)
    return {};
  return path;
}

int calcWeights(std::vector<int> bfsPathNodes, const std::unordered_map<int, Node> &graph)
{
  int edgeSum = 0;
  if (bfsPathNodes.empty())
    return 0; /// Lista vacia
  if (bfsPathNodes.size() == 1)
    return 0; /// Origen == Destino

  // Recorremos pares (u -> v) y buscamos en u el vecino v para sumar su peso
  for (size_t i = 0; i + 1 < bfsPathNodes.size(); ++i)
  {
    int u = bfsPathNodes[i];
    int v = bfsPathNodes[i + 1];

    // Si 'u' no existe como clave en el grafo, no hay aristas registradas desde u
    if (graph.count(u) == 0)
    {
      // No podemos sumar peso inexistente; asumimos 0 para esa arista
      continue;
    }

    const Node &node = graph.at(u);
    // Buscar en la lista de vecinos de 'u' el vecino 'v' y obtener su peso
    for (size_t j = 0; j < node.neighbors.size(); ++j)
    {
      if (node.neighbors[j].first == v)
      {
        edgeSum += node.neighbors[j].second; /// Sumar peso
        break;
      }
    }
    // Si no se encontró la arista explicita (u->v) asumimos peso 0 y continua
  }
  return edgeSum;
}

std::vector<int> prim_camino(const std::unordered_map<int, Node> &grafo, int origen, int destino)
{
  // Si no existen
  if (grafo.find(origen) == grafo.end() ||
      grafo.find(destino) == grafo.end())
  {
    return {};
  }

  std::unordered_map<int, int> parent;
  std::unordered_map<int, int> dist;
  std::unordered_map<int, bool> inMST;

  // Inicialización
  for (auto &p : grafo)
  {
    dist[p.first] = std::numeric_limits<int>::max();
    parent[p.first] = -1;
    inMST[p.first] = false;
  }

  // Cola de prioridad: (peso, nodo)
  using pii = std::pair<int, int>;
  std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

  dist[origen] = 0;
  pq.push({0, origen});

  while (!pq.empty())
  {
    int u = pq.top().second;
    pq.pop();

    if (inMST[u])
      continue;
    inMST[u] = true;

    // Si ya llegamos al destino en el MST, igual seguimos
    // hasta terminar el MST local necesario para conocer los padres.
    // Podrías cortar aquí, pero lo dejo completo por seguridad.

    for (auto &[v, w] : grafo.at(u).neighbors)
    {
      if (!inMST[v] && w < dist[v])
      {
        dist[v] = w;
        parent[v] = u;
        pq.push({dist[v], v});
      }
    }
  }

  // Reconstrucción del camino: destino → origen usando parent[]
  std::vector<int> camino;
  int actual = destino;

  while (actual != -1)
  {
    camino.push_back(actual);
    if (actual == origen)
      break;
    actual = parent[actual];
  }

  if (camino.back() != origen)
  {
    // No existe camino (grafo no conectado)
    return {};
  }

  // El camino quedó al revés, lo invertimos
  std::reverse(camino.begin(), camino.end());
  return camino;
}

void dijkstra_con_camino(const std::unordered_map<int, Node> &grafo,
                         int origen,
                         std::unordered_map<int, int> &distancias,
                         std::unordered_map<int, int> &predecesores)
{
  const int INF = std::numeric_limits<int>::max();

  distancias.clear();
  predecesores.clear();

  // Inicializar distancias a INF
  for (const auto &kv : grafo)
  {
    int id = kv.first;
    distancias[id] = INF;
  }

  using Par = std::pair<int, int>; // (distancia, nodo)
  std::priority_queue<Par, std::vector<Par>, std::greater<Par>> pq;

  distancias[origen] = 0;
  predecesores[origen] = -1;
  pq.push({0, origen});

  while (!pq.empty())
  {
    auto [dist, actual] = pq.top();
    pq.pop();

    // Si ya sabemos una mejor distancia, ignoramos esta entrada
    if (dist > distancias[actual])
      continue;

    auto itNodo = grafo.find(actual);
    if (itNodo == grafo.end())
      continue;

    const Node &node = itNodo->second;

    // Relajar aristas
    for (const auto &par : node.neighbors)
    {
      int vecino = par.first;
      int peso = par.second;

      if (dist + peso < distancias[vecino])
      {
        distancias[vecino] = dist + peso;
        predecesores[vecino] = actual;
        pq.push({distancias[vecino], vecino});
      }
    }
  }
}

std::vector<int> reconstruir_camino_dijkstra(
    int origen,
    int destino,
    const std::unordered_map<int, int> &predecesores)
{
  std::vector<int> camino;

  // Si el destino no tiene predecesor y no es el origen, no hay camino
  auto it = predecesores.find(destino);
  if (it == predecesores.end() && destino != origen)
  {
    return {};
  }

  int actual = destino;
  while (actual != -1)
  {
    camino.push_back(actual);
    auto it2 = predecesores.find(actual);
    if (it2 == predecesores.end())
      break; // seguridad
    actual = it2->second;
  }

  std::reverse(camino.begin(), camino.end());

  if (camino.empty() || camino.front() != origen)
  {
    // No se logró partir realmente del origen
    return {};
  }

  return camino;
}

std::vector<int> ruta_dijkstra(const std::unordered_map<int, Node> &grafo,
                               int origen, int destino)
{
  std::unordered_map<int, int> distancias;
  std::unordered_map<int, int> predecesores;

  dijkstra_con_camino(grafo, origen, distancias, predecesores);

  // Verificar si el destino es alcanzable
  if (distancias.find(destino) == distancias.end() ||
      distancias[destino] == std::numeric_limits<int>::max())
  {
    return {};
  }

  return reconstruir_camino_dijkstra(origen, destino, predecesores);
}

// BFS INVERTIDO: distAristas[n] = número mínimo de aristas de n→Base
static std::unordered_map<int, int>
bfs_invertido_distancias(const std::unordered_map<int, Node> &grafo, int base)
{
  std::unordered_map<int, int> dist;
  for (auto &p : grafo)
    dist[p.first] = std::numeric_limits<int>::max();

  /// Crear grafo invertido: v → u
  std::unordered_map<int, std::vector<int>> inv;
  for (auto &u : grafo)
    for (auto &e : u.second.neighbors)
      inv[e.first].push_back(u.first);

  /// BFS típico, idéntico a tu estilo
  std::queue<int> q;
  std::unordered_set<int> visited;

  q.push(base);
  visited.insert(base);
  dist[base] = 0;

  while (!q.empty())
  {
    int actual = q.front();
    q.pop();

    /// si no tiene entradas en el grafo invertido, continue
    if (inv.count(actual) == 0)
      continue;

    for (int padre : inv[actual])
    {
      if (visited.find(padre) == visited.end())
      {
        visited.insert(padre);
        dist[padre] = dist[actual] + 1;
        q.push(padre);
      }
    }
  }

  return dist;
}

// ============================================================
// A* usando heurística B (distAristas * w_min)
// ============================================================
std::vector<int> ruta_astar(const std::unordered_map<int, Node> &grafo,
                            int origen, int destino)
{
  if (!grafo.count(origen) || !grafo.count(destino))
    return {};

  // ==========================
  // 1) Encontrar Base
  // ==========================
  int base = -1;
  for (auto &p : grafo)
    if (p.second.tipo == 1)
      base = p.first;

  if (base == -1)
    return {};

  // ==========================
  // 2) Peso mínimo global
  // ==========================
  int w_min = std::numeric_limits<int>::max();
  for (auto &p : grafo)
    for (auto &e : p.second.neighbors)
      w_min = std::min(w_min, e.second);

  // ==========================
  // 3) DistAristas con BFS invertido
  // ==========================
  auto distAristas = bfs_invertido_distancias(grafo, base);

  auto heuristic = [&](int n) -> double
  {
    if (distAristas[n] == std::numeric_limits<int>::max())
      return std::numeric_limits<double>::infinity();
    return double(distAristas[n]) * double(w_min);
  };

  // ==========================
  // 4) A*
  // ==========================
  std::unordered_map<int, double> gScore, fScore;
  std::unordered_map<int, int> cameFrom;

  for (auto &p : grafo)
  {
    gScore[p.first] = std::numeric_limits<double>::infinity();
    fScore[p.first] = std::numeric_limits<double>::infinity();
  }

  gScore[origen] = 0.0;
  fScore[origen] = heuristic(origen);

  using PQItem = std::pair<double, int>;
  std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> openSet;
  openSet.emplace(fScore[origen], origen);

  while (!openSet.empty())
  {
    int actual = openSet.top().second;
    openSet.pop();

    if (actual == destino)
    {
      // reconstrucción típica
      std::vector<int> ruta;
      int t = destino;
      while (cameFrom.count(t))
      {
        ruta.push_back(t);
        t = cameFrom[t];
      }
      ruta.push_back(origen);
      std::reverse(ruta.begin(), ruta.end());
      return ruta;
    }

    for (auto &edge : grafo.at(actual).neighbors)
    {
      int nxt = edge.first;
      double w = edge.second;

      double tentative = gScore[actual] + w;

      if (tentative < gScore[nxt])
      {
        cameFrom[nxt] = actual;
        gScore[nxt] = tentative;
        fScore[nxt] = tentative + heuristic(nxt);
        openSet.emplace(fScore[nxt], nxt);
      }
    }
  }

  return {};
}
