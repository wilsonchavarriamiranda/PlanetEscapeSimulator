#ifndef RENDERIZADOR_MAPA_H
#define RENDERIZADOR_MAPA_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include "Node.h"
#include "UIButton.h"
#include "Maquina.h"

#define BTN_ANCHO 160.f
#define BTN_ALTO 45.f
#define BTN_OFFSET_X (-80.f)
#define BTN_OFFSET_REINICIAR_Y 80.f
#define BTN_OFFSET_SALIR_Y 130.f

struct PipelineAnim
{
  std::vector<int> ruta;
  float progreso = 0.f;
  float velocidad = 0.25f;
};

class RenderizadorMapa
{
private:
  std::unordered_map<int, PipelineAnim> pipelines;

  bool construirPresionado = false;

  const std::unordered_map<int, Node> &grafo;

  sf::RenderWindow window;
  sf::Font font;

  float radioNodo = 15.f;

  float escala = 1.f;
  float offsetX = 0.f;
  float offsetY = 0.f;

  sf::Color colorBase = sf::Color::Blue;
  sf::Color colorRecurso = sf::Color::Green;
  sf::Color colorVacio = sf::Color(150, 150, 150);
  sf::Color colorJugador = sf::Color::Yellow;

  bool clickNodo = false;
  int nodoSeleccionado = -1;
  int nodoActivo = -1;

  // =========================
  // GAME OVER
  // =========================
  bool modoGameOver = false;
  std::string mensajeGameOver = "";
  float gameOverAlpha = 0.f;

  std::unique_ptr<UIButton> btnReiniciar;
  std::unique_ptr<UIButton> btnSalir;

  // =========================
  // SELECCIÓN DE MÁQUINA (NUEVO SISTEMA)
  // =========================
  bool modoSeleccionMaquina = false; // similar a modoGameOver
  float seleccionAlpha = 0.f;        // fade-in local
  int tipoSeleccionado = -1;         // 1 = BFS, 2 = DFS

  std::unique_ptr<UIButton> btnBFS;      // ya existía en tu .h
  std::unique_ptr<UIButton> btnDFS;      // ya existía en tu .h
  std::unique_ptr<UIButton> btnCancelar; // NUEVO

  // =========================
  // BOTONES NORMALES
  // =========================
  std::unique_ptr<UIButton> btnConstruir;
  std::unique_ptr<UIButton> btnMejorar;

  std::unordered_map<int, Maquina> maquinas;
  bool mostrarBotonesMaquinas = false;
  int baseID = 0;

  std::vector<int> rutaMaquina(const Maquina &maq) const;
  double eficienciaPorNivel(int nivel) const;

public:
  RenderizadorMapa(const std::unordered_map<int, Node> &g);

  // (OBSOLETO: modal bloqueante)
  int dibujarSeleccionTipoMaquinaOverlay();

  void activarSeleccionMaquina();
  void dibujarSeleccionMaquinaOverlay();
  int obtenerResultadoSeleccionMaquina() const { return tipoSeleccionado; }
  void limpiarResultadoSeleccionMaquina() { tipoSeleccionado = -1; }

  void centrarVentana();
  bool ventanaAbierta() const { return window.isOpen(); }
  void procesarEventos();
  void limpiar();
  void mostrar();

  void dibujarGrafo();
  void dibujarJugador(int nodoActual);
  void dibujarCamino(const std::vector<int> &ruta, sf::Color color, float grosor = 4.f);

  void dibujarPanelRecursos();
  void dibujarPanelEstado(int recursosActuales, int bateria, int meta);
  void dibujarPanelMaquinas();
  void dibujarPanelCostos();

  void setmostrarBotonesMaquinas(bool mostrar)
  {
    mostrarBotonesMaquinas = mostrar;
  }

  bool botonConstruirPresionado() const { return construirPresionado; }
  void consumirBotonConstruir() { construirPresionado = false; }
  void manejarClickConstruir();

  void setNodoActivo(int nodo) { nodoActivo = nodo; }
  int getNodoActivo() const { return nodoActivo; }

  std::function<void()> onMaquinaConstruida;

  bool tieneMaquina(int nodo) const;
  int nivelMaquina(int nodo) const;
  void construirMaquina(int nodo, int tipo);
  void mejorarMaquina(int nodo);
  void reiniciarMaquinas() { maquinas.clear(); }
  const std::unordered_map<int, Maquina> &obtenerMaquinas() const { return maquinas; }

  void activarPipeline(int nodoRecurso, const std::vector<int> &ruta);
  void dibujarPipelines();
  const std::unordered_map<int, PipelineAnim> &getPipelines() const { return pipelines; }
  void limpiarPipelines();

  int *recursosExternos = nullptr;
  int *bateriaExterna = nullptr;

  sf::Clock relojProduccion;
  float intervaloProduccion = 1.0f;
  int costoConstruccionNivel1 = 25;
  int costoMejoras[5] = {0, 400, 600, 800, 1500};

  void conectarRecursos(int *ref);
  void conectarBateria(int *ref);

  void procesarEconomia();
  int produccionTotalTick();
  bool intentarConstruir(int nodo, int tipoInicial);
  bool intentarMejorar(int nodo);
  void restarRecursos(int cant);

  void activarGameOver();
  bool gameOverActivo() const { return modoGameOver; }
  void setMensajeGameOver(const std::string &msg) { mensajeGameOver = msg; }
  void dibujarGameOverOverlay();

  void updateUI();
  void drawUI();

  bool hayClickNodo() const { return clickNodo; }
  int obtenerNodoClick() const { return nodoSeleccionado; }
  void consumirClickNodo()
  {
    clickNodo = false;
    nodoSeleccionado = -1;
  }
};

#endif // RENDERIZADOR_MAPA_H
