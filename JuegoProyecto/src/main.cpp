/**
 * @file main.cpp
 * @brief Juego de nodos y recursos con grafo, movimientos y máquinas usando SFML.
 *
 * Este archivo contiene la lógica principal del juego, incluyendo:
 * - Generación o carga de grafos.
 * - Algoritmos de rutas (BFS, DFS, Prim, Dijkstra, A*).
 * - Manejo de jugador, batería y recursos.
 * - Renderización de grafos y UI con SFML.
 */

#include "Grafo.h"
#include "AlgoritmosGrafo.h"
#include "RenderizadorMapa.h"
#include "GeneradorMapa.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

/// Namespace anónimo para encapsular la lógica auxiliar
namespace
{
  /// Recursos iniciales del jugador
  constexpr int RECURSOS_INICIALES = 25;

  /**
   * @struct InputFlags
   * @brief Flags para configurar el input del juego desde la línea de comandos.
   */
  struct InputFlags
  {
    bool usarArchivo = false;                     /**< Si se debe cargar el grafo desde un archivo */
    std::string rutaArchivo = "assets/Grafo.txt"; /**< Ruta al archivo de grafo */
    bool usarRandom = true;                       /**< Si se debe generar un grafo aleatorio */
    int nodosRandom = 50;                         /**< Cantidad de nodos aleatorios a generar */
  };

  /**
   * @struct GameConfig
   * @brief Configuración estática del juego
   */
  struct GameConfig
  {
    int baseID = 0;             /**< ID del nodo base */
    int destinoID = 10;         /**< ID del nodo destino */
    int bateriaInicial = 100;   /**< Batería inicial del jugador */
    int metaRecursos = 1000000; /**< Cantidad de recursos para ganar */
  };

  /**
   * @struct GameState
   * @brief Estado dinámico del juego
   */
  struct GameState
  {
    int jugadorNodo = 0;                       /**< Nodo actual del jugador */
    int recursosActuales = RECURSOS_INICIALES; /**< Recursos actuales del jugador */
    int bateriaMaxima = 0;                     /**< Batería máxima */
    int bateriaActual = 0;                     /**< Batería actual */
    bool gameOver = false;                     /**< Indica si el juego terminó */
    bool winCondition = false;                 /**< Indica si se alcanzó condición de victoria */
  };

  /**
   * @brief Parsea los argumentos de la línea de comandos
   * @param argc Cantidad de argumentos
   * @param argv Vector de argumentos
   * @return InputFlags configurado según argumentos
   *
   * Argumentos soportados:
   * --archivo        → cargar grafo desde archivo
   * --random         → generar grafo aleatorio
   * --file <ruta>    → ruta de archivo para grafo
   * --n <cantidad>   → cantidad de nodos aleatorios
   */
  InputFlags parseArgs(int argc, char *argv[])
  {
    InputFlags flags;

    for (int i = 1; i < argc; ++i)
    {
      std::string arg = argv[i];

      if (arg == "--archivo")
      {
        flags.usarArchivo = true;
        flags.usarRandom = false;
      }
      else if (arg == "--random")
      {
        flags.usarArchivo = false;
        flags.usarRandom = true;
      }
      else if (arg == "--file" && i + 1 < argc)
      {
        flags.rutaArchivo = argv[++i];
      }
      else if (arg == "--n" && i + 1 < argc)
      {
        flags.nodosRandom = std::stoi(argv[++i]);
      }
    }

    return flags;
  }

  /**
   * @brief Imprime un camino de nodos en consola
   * @param path Vector de IDs de nodos
   */
  void imprimirCamino(const std::vector<int> &path)
  {
    if (path.empty())
    {
      std::cout << "  (no existe camino)\n";
      return;
    }
    for (std::size_t i = 0; i < path.size(); ++i)
    {
      std::cout << path[i];
      if (i + 1 < path.size())
        std::cout << " -> ";
    }
    std::cout << "\n";
  }

  /**
   * @brief Verifica si el jugador está atrapado en un nodo sin suficiente batería
   * @param nodo ID del nodo actual
   * @param bateria Batería actual
   * @param g Grafo completo
   * @return true si no hay movimientos posibles, false en caso contrario
   */
  bool jugadorAtrapado(int nodo,
                       int bateria,
                       const std::unordered_map<int, Node> &g)
  {
    if (bateria <= 0)
      return true;

    const Node &n = g.at(nodo);
    for (auto &[v, costo] : n.neighbors)
    {
      (void)v;
      if (costo <= bateria)
        return false;
    }

    return true;
  }

  /**
   * @brief Inicializa el estado del juego con valores por defecto
   * @param state Referencia al estado del juego
   * @param cfg Configuración del juego
   */
  void inicializarEstadoJuego(GameState &state, const GameConfig &cfg)
  {
    state.jugadorNodo = cfg.baseID;
    state.recursosActuales = RECURSOS_INICIALES;
    state.bateriaMaxima = cfg.bateriaInicial;
    state.bateriaActual = state.bateriaMaxima;
    state.gameOver = false;
    state.winCondition = false;
  }

  /**
   * @brief Actualiza la condición de victoria según los recursos actuales
   * @param state Referencia al estado del juego
   * @param cfg Configuración del juego
   */
  void actualizarWinCondition(GameState &state, const GameConfig &cfg)
  {
    if (state.recursosActuales >= cfg.metaRecursos)
      state.winCondition = !state.winCondition;
  }

  /**
   * @brief Ejecuta y muestra caminos usando BFS, DFS, Prim, Dijkstra y A*
   * @param grafo Grafo a evaluar
   * @param cfg Configuración del juego
   */
  void probarAlgoritmos(const Grafo &grafo, const GameConfig &cfg)
  {
    const auto &nodos = grafo.obtenerNodos();

    std::cout << "Probando BFS y DFS desde recurso " << cfg.destinoID
              << " hacia base " << cfg.baseID << "\n\n";

    auto caminoBFS = ruta_bfs(nodos, cfg.destinoID, cfg.baseID);
    std::cout << "Camino BFS:\n";
    imprimirCamino(caminoBFS);
    int pesoBFS = calcWeights(caminoBFS, nodos);
    std::cout << "Peso total BFS = " << pesoBFS << "\n\n";

    auto caminoDFS = ruta_dfs(nodos, cfg.destinoID, cfg.baseID);
    std::cout << "Camino DFS:\n";
    imprimirCamino(caminoDFS);
    int pesoDFS = calcWeights(caminoDFS, nodos);
    std::cout << "Peso total DFS = " << pesoDFS << "\n\n";

    auto caminoPrim = prim_camino(nodos, cfg.destinoID, cfg.baseID);
    std::cout << "Camino Prim:\n";
    imprimirCamino(caminoPrim);
    int pesoPrim = calcWeights(caminoPrim, nodos);
    std::cout << "Peso total Prim = " << pesoPrim << "\n\n";

    auto caminoDij = ruta_dijkstra(nodos, cfg.destinoID, cfg.baseID);
    std::cout << "Camino Dijkstra:\n";
    imprimirCamino(caminoDij);
    int pesoDij = calcWeights(caminoDij, nodos);
    std::cout << "Peso total Dijkstra = " << pesoDij << "\n\n";

    auto caminoAstar = ruta_astar(nodos, cfg.destinoID, cfg.baseID);
    std::cout << "Camino A*:\n";
    imprimirCamino(caminoAstar);
    int pesoAstar = calcWeights(caminoAstar, nodos);
    std::cout << "Peso total A* = " << pesoAstar << "\n\n";
  }

  /**
   * @brief Maneja un click sobre un nodo para mover al jugador
   * @param state Estado del juego
   * @param cfg Configuración del juego
   * @param renderer Renderizador del mapa
   * @param grafo Grafo completo
   */
  void manejarClickNodo(GameState &state,
                        const GameConfig &cfg,
                        RenderizadorMapa &renderer,
                        const Grafo &grafo)
  {
    if (state.gameOver || !renderer.hayClickNodo())
      return;

    int clic = renderer.obtenerNodoClick();
    bool esVecino = false;
    int costo = 0;

    const auto &nodos = grafo.obtenerNodos();
    const Node &nodoActual = nodos.at(state.jugadorNodo);

    for (auto &[v, w] : nodoActual.neighbors)
    {
      if (v == clic)
      {
        esVecino = true;
        costo = w;
        break;
      }
    }

    if (!esVecino || state.bateriaActual < costo)
    {
      std::cout << (!esVecino ? "No es adyacente\n" : "Batería insuficiente\n");
      renderer.consumirClickNodo();
      return;
    }

    state.bateriaActual -= costo;
    state.jugadorNodo = clic;
    renderer.setNodoActivo(state.jugadorNodo);

    const Node &nodoDestino = nodos.at(state.jugadorNodo);
    int tipoNodo = nodoDestino.tipo;

    renderer.setmostrarBotonesMaquinas(tipoNodo == 2);

    if (state.jugadorNodo == cfg.baseID)
      state.bateriaActual = state.bateriaMaxima;
    else if (renderer.tieneMaquina(state.jugadorNodo))
      state.bateriaActual = std::max(static_cast<int>(state.bateriaMaxima * 0.60),
                                     state.bateriaActual);

    else if (state.bateriaActual <= 0 || jugadorAtrapado(state.jugadorNodo, state.bateriaActual, nodos))
    {
      state.gameOver = true;
      renderer.setMensajeGameOver(
          state.bateriaActual <= 0
              ? "Batería agotada lejos de la base"
              : "Sin movimientos posibles");

      renderer.activarGameOver();
    }

    renderer.consumirClickNodo();
  }

  /**
   * @brief Reinicia el juego si la UI no muestra Game Over pero el estado lo indica
   * @param state Estado del juego
   * @param cfg Configuración del juego
   * @param renderer Renderizador del mapa
   */
  void reiniciarJuegoSiCorresponde(GameState &state,
                                   const GameConfig &cfg,
                                   RenderizadorMapa &renderer)
  {
    if (!renderer.gameOverActivo() && state.gameOver)
    {
      std::cout << ">>> Juego reiniciado\n";
      inicializarEstadoJuego(state, cfg);
      renderer.reiniciarMaquinas();
      renderer.limpiarPipelines();
      renderer.setNodoActivo(state.jugadorNodo);
    }
  }

  /**
   * @brief Dibuja un frame completo del juego
   * @param state Estado del juego
   * @param cfg Configuración del juego
   * @param renderer Renderizador del mapa
   */
  void dibujarFrame(const GameState &state,
                    const GameConfig &cfg,
                    RenderizadorMapa &renderer)
  {
    renderer.limpiar();
    renderer.dibujarPipelines();
    renderer.dibujarGrafo();
    renderer.dibujarJugador(state.jugadorNodo);
    renderer.dibujarPanelEstado(state.recursosActuales, state.bateriaActual, cfg.metaRecursos);
    renderer.dibujarPanelRecursos();
    renderer.dibujarPanelMaquinas();
    renderer.dibujarPanelCostos();
    renderer.updateUI();
    renderer.drawUI();
    renderer.mostrar();
  }

  /**
   * @brief Loop principal del juego
   * @param renderer Renderizador del mapa
   * @param grafo Grafo del juego
   * @param cfg Configuración del juego
   * @param state Estado del juego
   */
  void ejecutarJuego(RenderizadorMapa &renderer,
                     const Grafo &grafo,
                     const GameConfig &cfg,
                     GameState &state)
  {
    renderer.centrarVentana();

    while (renderer.ventanaAbierta())
    {
      actualizarWinCondition(state, cfg);
      if (state.winCondition)
      {
        std::cout << "GANASTE!" << std::endl;
        break;
      }

      renderer.procesarEventos();
      renderer.procesarEconomia();
      manejarClickNodo(state, cfg, renderer, grafo);
      reiniciarJuegoSiCorresponde(state, cfg, renderer);
      if (!renderer.gameOverActivo()) // no interferir con GameOver
      {
        int tipo = renderer.obtenerResultadoSeleccionMaquina();
        if (tipo > 0)
        {
          // Construir máquina en el nodo activo
          renderer.intentarConstruir(renderer.getNodoActivo(), tipo);

          // Limpiar selección (importante)
          renderer.limpiarResultadoSeleccionMaquina();

          std::cout << "Máquina creada tipo " << tipo
                    << " en nodo " << renderer.getNodoActivo() << "\n";
        }
      }

      dibujarFrame(state, cfg, renderer);
    }
  }

}

/**
 * @brief Función principal
 * @param argc Cantidad de argumentos
 * @param argv Vector de argumentos
 * @return 0 si terminó correctamente
 *
 * Genera o carga el grafo, inicializa el renderizador, estado del juego y ejecuta el loop principal.
 */
int main(int argc, char *argv[])
{
  // ------------------------
  // Generar mapa procedural o cargar mapa de archivo
  InputFlags flags = parseArgs(argc, argv);

  std::vector<NodoInfo> nodosInfo;
  std::vector<AristaInfo> aristasInfo;

  if (flags.usarArchivo)
  {
    std::cout << "Cargando grafo desde archivo: " << flags.rutaArchivo << "\n";

    if (!leerNodos(flags.rutaArchivo, nodosInfo, aristasInfo))
    {
      std::cerr << "Error cargando el grafo desde archivo.\n";
      return 1;
    }

    std::cout << "Archivo cargado correctamente.\n";
  }
  else
  {
    std::cout << "Generando grafo procedural con " << flags.nodosRandom << " nodos...\n";

    GeneradorMapa::Config cfgMapa;
    cfgMapa.totalNodos = flags.nodosRandom;
    cfgMapa.cantRecursos = 10;
    cfgMapa.radioNodo = 25.f;
    cfgMapa.multiplicadorDistancia = 4.5f;

    GeneradorMapa::generarMapa(nodosInfo, aristasInfo, cfgMapa);

    std::cout << "Mapa aleatorio generado correctamente.\n";
  }

  std::cout << "Nodos: " << nodosInfo.size() << "\n";
  std::cout << "Aristas: " << aristasInfo.size() << "\n\n";

  // ------------------------
  // Construir el grafo
  Grafo grafo;
  grafo.construir(nodosInfo, aristasInfo);

  // Configuración del juego
  GameConfig cfgJuego;
  cfgJuego.baseID = 0;
  cfgJuego.destinoID = 10;
  cfgJuego.bateriaInicial = 100;
  cfgJuego.metaRecursos = 1'000'000;

  // Probar algoritmos de caminos antes de iniciar el juego
  probarAlgoritmos(grafo, cfgJuego);

  // ------------------------
  // Inicializar renderizador y estado del juego
  RenderizadorMapa renderer(grafo.obtenerNodos());

  GameState state;
  inicializarEstadoJuego(state, cfgJuego);

  renderer.conectarRecursos(&state.recursosActuales);
  renderer.conectarBateria(&state.bateriaActual);

  renderer.onMaquinaConstruida = [&state]()
  {
    state.bateriaMaxima += 50;
    std::cout << "[+] Máquina construida → Nueva batería máxima = "
              << state.bateriaMaxima << "\n";
    state.bateriaActual = std::min(state.bateriaActual, state.bateriaMaxima);
  };

  // ------------------------
  // Ejecutar loop del juego
  ejecutarJuego(renderer, grafo, cfgJuego, state);

  return 0;
}
