#include <iostream>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "RenderizadorMapa.h"
#include "AlgoritmosGrafo.h"
#include "WindowConfig.h"

double RenderizadorMapa::eficienciaPorNivel(int nivel) const
{
  switch (nivel)
  {
  case 1:
    return 0.50; // 50%
  case 2:
    return 0.60; // 60%
  case 3:
    return 0.75; // 75%
  case 4:
    return 1.00; // 100%
  default:
    return 0.0;
  }
}

std::vector<int> RenderizadorMapa::rutaMaquina(const Maquina &maq) const
{
  int origen = maq.getNodo(); // nodo recurso
  int destino = baseID;

  int nivel = maq.getNivel();

  // Interpretamos el nivel de la máquina como el "nivel tecnológico":
  // N1 → BFS o DFS, N2 → Prim/Greedy, N3 → Dijkstra, N4 → A*
  switch (nivel)
  {
  case 1:
    return ruta_bfs(grafo, origen, destino);
  case 2:
    // prim_camino como aproximación greedy
    return prim_camino(grafo, origen, destino);
  case 3:
    return ruta_dijkstra(grafo, origen, destino);
  case 4:
    return ruta_astar(grafo, origen, destino);
  default:
    return ruta_dijkstra(grafo, origen, destino);
  }
}

void RenderizadorMapa::conectarRecursos(int *ref)
{
  recursosExternos = ref;
}
void RenderizadorMapa::conectarBateria(int *ref)
{
  bateriaExterna = ref;
}

void RenderizadorMapa::restarRecursos(int cant)
{
  if (!recursosExternos)
    return;
  *recursosExternos -= cant;
  if (*recursosExternos < 0)
    *recursosExternos = 0;
}

int RenderizadorMapa::produccionTotalTick()
{
  int total = 0;

  for (const auto &par : maquinas)
  {
    const Maquina &maq = par.second;

    int nodo = maq.getNodo();
    int nivel = maq.getNivel();

    const Node &recurso = grafo.at(nodo);

    double eff = eficienciaPorNivel(nivel);
    int valor = recurso.valor;

    std::vector<int> ruta = rutaMaquina(maq);
    int costoCamino = calcWeights(ruta, grafo);

    int prod = (int)(eff * valor) - costoCamino;
    if (prod < 0)
      prod = 0;

    total += prod;
  }

  return total;
}

void RenderizadorMapa::procesarEconomia()
{
  if (!recursosExternos)
    return;

  if (relojProduccion.getElapsedTime().asSeconds() >= intervaloProduccion)
  {
    relojProduccion.restart();

    int prod = produccionTotalTick();
    *recursosExternos += prod;
  }
}

bool RenderizadorMapa::intentarConstruir(int nodo, int tipoInicial)
{
  if (!recursosExternos)
    return false;
  if (tieneMaquina(nodo))
    return false;
  if (*recursosExternos < this->costoConstruccionNivel1)
    return false;

  restarRecursos(this->costoConstruccionNivel1);

  // CREAR UNA SOLA MÁQUINA
  maquinas[nodo] = Maquina(nodo, tipoInicial, 1);

  int nivel = maquinas[nodo].getNivel();

  std::vector<int> ruta;
  switch (nivel)
  {
  case 1:
    ruta = ruta_bfs(grafo, nodo, baseID);
    break;
  case 2:
    ruta = prim_camino(grafo, nodo, baseID);
    break;
  case 3:
    ruta = ruta_dijkstra(grafo, nodo, baseID);
    break;
  case 4:
    ruta = ruta_astar(grafo, nodo, baseID);
    break;
  }

  bateriaExterna += 50;
  activarPipeline(nodo, ruta);
  if (this->onMaquinaConstruida)
    onMaquinaConstruida();

  return true;
}

bool RenderizadorMapa::intentarMejorar(int nodo)
{
  auto it = maquinas.find(nodo);
  if (it == maquinas.end())
    return false;

  int nivelActual = it->second.getNivel();
  if (nivelActual >= 4)
    return false;

  int costo = costoMejoras[nivelActual];
  if (*recursosExternos < costo)
    return false;

  restarRecursos(costo);

  // SUBIR NIVEL
  it->second.sumNivel();
  int nuevoNivel = it->second.getNivel();

  std::vector<int> ruta;

  // USAR EL NUEVO NIVEL PARA SELECCIONAR ALGORITMO
  switch (nuevoNivel)
  {
  case 1:
    ruta = ruta_bfs(grafo, nodo, baseID);
    break;
  case 2:
    ruta = prim_camino(grafo, nodo, baseID);
    break;
  case 3:
    ruta = ruta_dijkstra(grafo, nodo, baseID);
    break;
  case 4:
    ruta = ruta_astar(grafo, nodo, baseID);
    break;
  }

  // REEMPLAZAR SIEMPRE EL PIPELINE ANTERIOR
  activarPipeline(nodo, ruta);

  return true;
}

void RenderizadorMapa::activarPipeline(int nodoRecurso, const std::vector<int> &ruta)
{
  PipelineAnim p;
  p.ruta = ruta;
  p.progreso = 0.f;
  p.velocidad = 0.4f; // puedes ajustarlo para más rápido/lento

  pipelines[nodoRecurso] = p;
}

void RenderizadorMapa::dibujarPipelines()
{
  for (auto &kv : pipelines)
  {
    PipelineAnim &p = kv.second;
    if (p.ruta.size() < 2)
      continue;

    // avanzar progreso
    p.progreso += p.velocidad * 0.016f; // 60FPS approx
    if (p.progreso > 1.f)
      p.progreso = 0.f;

    // Línea base del pipeline
    for (size_t i = 0; i + 1 < p.ruta.size(); ++i)
    {
      int u = p.ruta[i];
      int v = p.ruta[i + 1];

      sf::Vector2f a(grafo.at(u).x * escala + offsetX,
                     grafo.at(u).y * escala + offsetY);

      sf::Vector2f b(grafo.at(v).x * escala + offsetX,
                     grafo.at(v).y * escala + offsetY);

      sf::Vertex linea[] = {
          sf::Vertex(a, sf::Color(0, 180, 255, 150)),
          sf::Vertex(b, sf::Color(0, 180, 255, 150))};
      window.draw(linea, 2, sf::Lines);

      // Animación del “punto” — transporte de recurso
      float t = p.progreso;
      float px = a.x * (1 - t) + b.x * t;
      float py = a.y * (1 - t) + b.y * t;

      sf::CircleShape dot(5);
      dot.setFillColor(sf::Color::White);
      dot.setOrigin(5, 5);
      dot.setPosition(px, py);
      window.draw(dot);
    }
  }
}
void RenderizadorMapa::limpiarPipelines()
{
  this->pipelines.clear();
}

RenderizadorMapa::RenderizadorMapa(const std::unordered_map<int, Node> &g)
    : grafo(g)
{
  sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

  unsigned int screenW = desktop.width;
  unsigned int screenH = desktop.height;

  // -----------------------------
  // 1) Tamaño requerido
  unsigned int winW = screenW - 15;
  unsigned int winH = screenH - 40;

  // Leer window.cfg ANTES de create
  int posX = 5, posY = 5;
  {
    std::ifstream cfg("window.cfg");
    if (cfg.good())
      cfg >> posX >> posY;
  }

  // -----------------------------
  // 2) Crear ventana SIN resize-
  window.create(
      sf::VideoMode(winW, winH),
      "Simulador del Planeta - Grafo",
      sf::Style::Titlebar | sf::Style::Close);

  // Cargar fuente
  if (!font.loadFromFile("assets/arial.ttf"))
    std::cout << "ERROR: No se pudo cargar arial.ttf\n";

  // -----------------------------
  // BOTONES DE MAQUINAS
  float margin = 10.f;

  // Debe coincidir con el panel Estado (ancho=170)
  float panelAncho = 170.f;

  // X alineado a la derecha, con un pequeño ajuste para centrar
  float botonesX = window.getSize().x - panelAncho - margin + 15.f;

  // Y arriba del panel Estado
  float botonesY = 20.f; // Si los quieres más arriba/bajo, ajusta este valor

  btnConstruir = std::make_unique<UIButton>(
      font,
      "Construir",
      sf::Vector2f(botonesX, botonesY),
      sf::Vector2f(140.f, 40.f));

  btnMejorar = std::make_unique<UIButton>(
      font,
      "Mejorar",
      sf::Vector2f(botonesX, botonesY + 50.f),
      sf::Vector2f(140.f, 40.f));

  // Acciones al hacer clic:
  btnConstruir->onClick = [this]()
  {
    if (nodoActivo == -1)
    {
      std::cout << "No hay nodo activo para construir.\n";
      return;
    }

    if (grafo.count(nodoActivo) == 0)
    {
      std::cout << "Nodo activo inválido: " << nodoActivo << "\n";
      return;
    }

    if (maquinas.count(nodoActivo) == 0)
    {
      activarSeleccionMaquina();
    }
    else
    {
      std::cout << "Ya existe una máquina en este nodo.\n";
    }
  };

  btnMejorar->onClick = [this]()
  {
    if (nodoActivo == -1)
    {
      std::cout << "No hay nodo activo para mejorar.\n";
      return;
    }

    // Verificar que el nodo existe en el grafo
    if (grafo.count(nodoActivo) == 0)
    {
      std::cout << "Nodo activo inválido: " << nodoActivo << "\n";
      return;
    }

    // Verificar que haya una máquina en el nodo
    if (maquinas.count(nodoActivo) == 0)
    {
      std::cout << "No hay máquina en el nodo " << nodoActivo << " para mejorar.\n";
      return;
    }
    intentarMejorar(nodoActivo);
    // Mejorar la máquina
    // mejorarMaquina(nodoActivo);
  };

  // -----------------------------
  // 3) Cargar posición desde window.cfg (si existe)
  bool usarConfig = false;
  sf::Vector2i posCFG(5, 5);

  {
    std::ifstream cfg("window.cfg");
    if (cfg.good())
    {
      int px, py;
      cfg >> px >> py;
      if (!cfg.fail())
      {
        posCFG = sf::Vector2i(px, py);
        usarConfig = true;
      }
    }
  }

  window.setVisible(false);               // Ocultamos
  window.setPosition(sf::Vector2i(0, 0)); // Forzamos una posición inicial
  window.setVisible(true);                // Mostramos ahora si acepta el move)
  window.display();                       // Render inicial permite mapear ventana

  // -----------------------------
  // 5) Ahora sí mover a la posición final
  if (usarConfig)
    window.setPosition(posCFG);
  else
    window.setPosition(sf::Vector2i(5, 5));

  // ---------------------------------------------------------
  // 6) Escalado y offsets del grafo
  if (grafo.empty())
  {
    escala = 1.f;
    offsetX = offsetY = 0.f;
    return;
  }

  for (const auto &[id, node] : grafo)
  {
    if (node.tipo == 1)
    {
      baseID = id;
      break;
    }
  }
  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::lowest();

  for (const auto &[id, node] : grafo)
  {
    minX = std::min(minX, (float)node.x);
    maxX = std::max(maxX, (float)node.x);
    minY = std::min(minY, (float)node.y);
    maxY = std::max(maxY, (float)node.y);
  }

  float rangoX = maxX - minX;
  float rangoY = maxY - minY;
  if (rangoX == 0)
    rangoX = 1;
  if (rangoY == 0)
    rangoY = 1;

  float margen = 50.f;
  float dibAncho = winW - 2 * margen;
  float dibAlto = winH - 2 * margen;

  float escalaX = dibAncho / rangoX;
  float escalaY = dibAlto / rangoY;
  escala = std::min(escalaX, escalaY);

  offsetX = margen - minX * escala;
  offsetY = margen - minY * escala;
}

void RenderizadorMapa::centrarVentana()
{
  sf::VideoMode desk = sf::VideoMode::getDesktopMode();

  int x = (desk.width - window.getSize().x) / 2;
  int y = (desk.height - window.getSize().y) / 2;

  window.setPosition(sf::Vector2i(x, y));
}

void RenderizadorMapa::procesarEventos()
{
  sf::Event e;
  while (window.pollEvent(e))
  {
    static bool fullscreen = false;
    static sf::Vector2i lastPos;  // posición antes de fullscreen
    static sf::Vector2u lastSize; // tamaño antes de fullscreen

    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F11)
    {
      fullscreen = !fullscreen;

      if (fullscreen)
      {
        // Guardamos posición y tamaño actuales ANTES de fullscreen
        lastPos = window.getPosition();
        lastSize = window.getSize();

        // Entrar a fullscreen
        window.create(
            sf::VideoMode::getDesktopMode(),
            "Simulador del Planeta - Grafo (Fullscreen)",
            sf::Style::Fullscreen);
      }
      else
      {
        // Salir de fullscreen → restaurar tamaño correcto
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        unsigned int winW = desktop.width - 15;
        unsigned int winH = desktop.height - 40;

        window.create(
            sf::VideoMode(winW, winH),
            "Simulador del Planeta - Grafo",
            sf::Style::Titlebar | sf::Style::Close);

        // Intentar restaurar posición guardada
        window.setVisible(false);
        window.setPosition(lastPos);
        window.setVisible(true);
      }
    }

    if (e.type == sf::Event::Closed)
    {
      sf::Vector2i pos = window.getPosition();
      std::ofstream out("window.cfg");
      out << pos.x << " " << pos.y;
      out.close();

      window.close();
    }

    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left)
    {
      float mx = static_cast<float>(e.mouseButton.x);
      float my = static_cast<float>(e.mouseButton.y);

      // ============================
      // 1) DETECCIÓN DE NODOS
      clickNodo = false;
      nodoSeleccionado = -1;

      if (!modoGameOver)
      {
        for (const auto &[id, node] : grafo)
        {
          float X = node.x * escala + offsetX;
          float Y = node.y * escala + offsetY;

          float dx = mx - X;
          float dy = my - Y;
          /// Click dentro de nodo
          if (dx * dx + dy * dy <= radioNodo * radioNodo)
          {
            clickNodo = true;
            nodoSeleccionado = id;
            break;
          }
        }
      }
    }
  }
}

void RenderizadorMapa::limpiar()
{
  window.clear(sf::Color::Black);
}

void RenderizadorMapa::mostrar()
{
  window.display();
}

void RenderizadorMapa::dibujarGrafo()
{
  // ==========================
  // 1. Dibujar aristas + pesos
  for (const auto &[id, node] : grafo)
  {
    for (const auto &[v, peso] : node.neighbors)
    {
      if (!grafo.count(v))
        continue;

      if (id > v) // evitar duplicar arista (grafo no dirigido)
        continue;

      sf::Vector2f p1(node.x * escala + offsetX,
                      node.y * escala + offsetY);
      sf::Vector2f p2(grafo.at(v).x * escala + offsetX,
                      grafo.at(v).y * escala + offsetY);

      sf::Vertex line[] = {
          sf::Vertex(p1, sf::Color::White),
          sf::Vertex(p2, sf::Color::White)};
      window.draw(line, 2, sf::Lines);

      // peso
      sf::Text wtext;
      wtext.setFont(font);
      wtext.setString(std::to_string(peso));
      wtext.setCharacterSize(14);
      wtext.setFillColor(sf::Color::Cyan);

      sf::Vector2f mid((p1.x + p2.x) / 2.f,
                       (p1.y + p2.y) / 2.f);

      sf::FloatRect wb = wtext.getLocalBounds();
      wtext.setOrigin(wb.width / 2.f, wb.height / 2.f);
      wtext.setPosition(mid.x, mid.y - 8.f);

      window.draw(wtext);
    }
  }

  // ==========================
  // 2. Dibujar nodos + ID
  for (const auto &[id, node] : grafo)
  {
    float X = node.x * escala + offsetX;
    float Y = node.y * escala + offsetY;

    sf::CircleShape c(radioNodo);
    c.setOrigin(radioNodo, radioNodo);
    c.setPosition(X, Y);

    if (node.tipo == 1)
      c.setFillColor(colorBase);
    else if (node.tipo == 2)
      c.setFillColor(colorRecurso);
    else
      c.setFillColor(colorVacio);

    window.draw(c);

    sf::Text text;
    text.setFont(font);
    text.setString(std::to_string(id));
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Black);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text.setPosition(X, Y);

    window.draw(text);
  }

  // =======================================
  // 3. Dibujar rutas de máquinas instaladas
  for (const auto &par : maquinas)
  {
    const Maquina &maq = par.second;
    std::vector<int> ruta = rutaMaquina(maq);

    if (ruta.size() < 2)
      continue;

    // Color según nivel de máquina
    sf::Color col;
    switch (maq.getNivel())
    {
    case 1:
      col = sf::Color(255, 0, 0, 220);
      break; // rojo
    case 2:
      col = sf::Color(255, 255, 0, 220);
      break; // amarillo
    case 3:
      col = sf::Color(0, 255, 0, 220);
      break; // verde
    case 4:
      col = sf::Color(0, 255, 255, 220);
      break; // cian
    default:
      col = sf::Color(255, 255, 255, 220);
      break;
    }

    // Reutilizamos dibujarCamino para que respete escala/offset
    dibujarCamino(ruta, col, 4.f);
  }
}

void RenderizadorMapa::dibujarJugador(int nodoActual)
{
  if (!grafo.count(nodoActual))
    return;

  const Node &node = grafo.at(nodoActual);
  float X = node.x * escala + offsetX;
  float Y = node.y * escala + offsetY;

  sf::CircleShape jugador(radioNodo + 4);
  jugador.setOrigin(radioNodo + 4, radioNodo + 4);
  jugador.setPosition(X, Y);
  jugador.setFillColor(colorJugador);

  window.draw(jugador);

  sf::Text text;
  text.setFont(font);
  text.setString(std::to_string(nodoActual));
  text.setCharacterSize(14);
  text.setFillColor(sf::Color::Black);

  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  text.setPosition(X, Y);

  window.draw(text);
}

void RenderizadorMapa::dibujarCamino(const std::vector<int> &ruta,
                                     sf::Color color,
                                     float grosor)
{
  (void)grosor; // no usado de momento

  if (ruta.size() < 2)
    return;

  for (size_t i = 0; i + 1 < ruta.size(); ++i)
  {
    int u = ruta[i];
    int v = ruta[i + 1];

    if (!grafo.count(u) || !grafo.count(v))
      continue;

    float x1 = grafo.at(u).x * escala + offsetX;
    float y1 = grafo.at(u).y * escala + offsetY;
    float x2 = grafo.at(v).x * escala + offsetX;
    float y2 = grafo.at(v).y * escala + offsetY;

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(x1, y1), color),
        sf::Vertex(sf::Vector2f(x2, y2), color)};

    window.draw(line, 2, sf::Lines);
  }
}

void RenderizadorMapa::dibujarPanelRecursos()
{
  float textSize = 12.f;
  float spacing = 14.f;

  int cantRecursos = 0;
  for (const auto &[id, node] : grafo)
    if (node.tipo == 2)
      cantRecursos++;

  float alto = 20.f + cantRecursos * spacing;
  float ancho = 130.f;
  float margin = 50.f;

  float x = window.getSize().x - ancho - margin;
  float y = window.getSize().y - alto - margin;

  sf::RectangleShape panel(sf::Vector2f(ancho, alto));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(0, 0, 0, 120));
  panel.setOutlineColor(sf::Color(200, 200, 200));
  panel.setOutlineThickness(1.f);
  window.draw(panel);

  sf::Text titulo;
  titulo.setFont(font);
  titulo.setCharacterSize(textSize + 1);
  titulo.setFillColor(sf::Color::Yellow);
  titulo.setString("Recursos");
  titulo.setPosition(x + 6, y + 3);
  window.draw(titulo);

  float offset = 20.f;

  for (const auto &[id, node] : grafo)
  {
    if (node.tipo != 2)
      continue;

    sf::Text linea;
    linea.setFont(font);
    linea.setCharacterSize(textSize);
    linea.setFillColor(sf::Color::White);
    linea.setString(std::to_string(id) + ": " + std::to_string(node.valor));
    linea.setPosition(x + 6, y + offset);

    offset += spacing;
    window.draw(linea);
  }
}

void RenderizadorMapa::dibujarPanelEstado(int recursosActuales, int bateria, int meta)
{
  float textSize = 12.f;
  float spacing = 14.f;
  float ancho = 130.f;
  float altoEstado = 20.f + 3 * spacing;
  float margin = 50.f;

  int cantRecursos = 0;
  for (const auto &[id, node] : grafo)
    if (node.tipo == 2)
      cantRecursos++;

  float spacingRec = 14.f;
  float altoRecursos = 20.f + cantRecursos * spacingRec;

  float yRecursos = window.getSize().y - altoRecursos - margin;

  float x = window.getSize().x - ancho - margin;
  float y = yRecursos - altoEstado - 5.f;

  sf::RectangleShape panel(sf::Vector2f(ancho, altoEstado));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(0, 0, 0, 140));
  panel.setOutlineColor(sf::Color(200, 200, 200));
  panel.setOutlineThickness(1.f);
  window.draw(panel);

  sf::Text titulo;
  titulo.setFont(font);
  titulo.setCharacterSize(textSize + 1);
  titulo.setFillColor(sf::Color::Cyan);
  titulo.setString("Estado");
  titulo.setPosition(x + 6, y + 3);
  window.draw(titulo);

  float offset = 20.f;

  sf::Text tRec;
  tRec.setFont(font);
  tRec.setCharacterSize(textSize);
  tRec.setFillColor(sf::Color::White);
  tRec.setString("Recursos: " + std::to_string(recursosActuales));
  tRec.setPosition(x + 6, y + offset);
  window.draw(tRec);
  offset += spacing;

  sf::Text tBat;
  tBat.setFont(font);
  tBat.setCharacterSize(textSize);
  tBat.setFillColor(sf::Color::White);
  tBat.setString("Bateria: " + std::to_string(bateria) + "%");
  tBat.setPosition(x + 6, y + offset);
  window.draw(tBat);
  offset += spacing;

  sf::Text tMeta;
  tMeta.setFont(font);
  tMeta.setCharacterSize(textSize);
  tMeta.setFillColor(sf::Color::White);
  tMeta.setString("Meta: " + std::to_string(meta));
  tMeta.setPosition(x + 6, y + offset);
  window.draw(tMeta);
}

void RenderizadorMapa::dibujarPanelMaquinas()
{
  // Contar recursos para ubicar panel
  int cantRecursos = 0;
  for (const auto &[id, node] : grafo)
    if (node.tipo == 2)
      cantRecursos++;

  float textSize = 12.f;
  float spacing = 14.f;
  float ancho = 150.f;
  float margin = 50.f;

  float altoMaquinas = 20.f + static_cast<float>(maquinas.size()) * spacing;
  float altoRecursos = 20.f + cantRecursos * spacing;
  float altoEstado = 20.f + 3 * spacing;

  float yRecursosEstado = window.getSize().y - altoRecursos - altoEstado - margin;
  float x = window.getSize().x - ancho - margin;
  float y = yRecursosEstado - altoMaquinas - 10.f;

  // Fondo
  sf::RectangleShape panel(sf::Vector2f(ancho, altoMaquinas));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(0, 0, 0, 140));
  panel.setOutlineColor(sf::Color(200, 200, 200));
  panel.setOutlineThickness(1.f);
  window.draw(panel);

  // Título
  sf::Text titulo;
  titulo.setFont(font);
  titulo.setCharacterSize(textSize + 1);
  titulo.setFillColor(sf::Color::Red);
  titulo.setString("Maquinas");
  titulo.setPosition(x + 6, y + 3);
  window.draw(titulo);

  float offset = 20.f;

  for (const auto &par : maquinas)
  {
    const Maquina &maq = par.second;
    int nodo = maq.getNodo();
    int nivel = maq.getNivel();

    // =============================
    // Calcular producción de máquina
    auto itNodo = grafo.find(nodo);
    if (itNodo == grafo.end())
      continue;

    const Node &nodoRecurso = itNodo->second;
    int valorRecurso = nodoRecurso.valor;

    std::vector<int> ruta = rutaMaquina(maq);
    int costoCamino = calcWeights(ruta, grafo);

    double eff = eficienciaPorNivel(nivel);
    int produccion = std::max(
        0,
        static_cast<int>(eff * static_cast<double>(valorRecurso)) - costoCamino);

    // =============================
    // Texto: Nodo, Nivel, Producción
    sf::Text linea;
    linea.setFont(font);
    linea.setCharacterSize(textSize);
    linea.setFillColor(sf::Color::White);

    // Ejemplo de formato: "N 7 L2 +35"
    linea.setString(
        "N" + std::to_string(nodo) +
        " Lvl" + std::to_string(nivel) +
        " +" + std::to_string(produccion) +
        "rps");

    linea.setPosition(x + 6, y + offset);
    offset += spacing;
    window.draw(linea);
  }
}

void RenderizadorMapa::dibujarPanelCostos()
{
  int cantRecursos = 0;
  for (const auto &[id, node] : grafo)
    if (node.tipo == 2)
      cantRecursos++;

  float textSize = 12.f;
  float spacing = 14.f;
  float ancho = 150.f;
  float margin = 50.f;

  float altoMaquina = 20.f + static_cast<float>(maquinas.size()) * spacing;
  float altoRecursos = 20.f + cantRecursos * spacing;
  float altoEstado = 20.f + 4 * spacing;
  float altoCosto = 20.f + 4 * spacing;

  float yRecursosEstadoMaquina = window.getSize().y - altoRecursos - altoEstado - altoMaquina - margin;
  float x = window.getSize().x - ancho - margin;
  float y = yRecursosEstadoMaquina - altoCosto;

  sf::RectangleShape panel(sf::Vector2f(ancho, altoCosto));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(0, 0, 0, 120));
  panel.setOutlineColor(sf::Color(200, 200, 200));
  panel.setOutlineThickness(1.f);
  window.draw(panel);

  sf::Text titulo;
  titulo.setFont(font);
  titulo.setCharacterSize(textSize + 1);
  titulo.setFillColor(sf::Color::Yellow);
  titulo.setString("Costos");
  titulo.setPosition(x + 6, y + 3);
  window.draw(titulo);

  float offset = 20.f;

  sf::Text tN1;
  tN1.setFont(font);
  tN1.setCharacterSize(textSize);
  tN1.setFillColor(sf::Color::White);
  tN1.setString("Nivel 1: " + std::to_string(costoConstruccionNivel1));
  tN1.setPosition(x + 6, y + offset);
  window.draw(tN1);
  offset += spacing;

  sf::Text tN2;
  tN2.setFont(font);
  tN2.setCharacterSize(textSize);
  tN2.setFillColor(sf::Color::White);
  tN2.setString("Nivel 2: " + std::to_string(costoMejoras[1]));
  tN2.setPosition(x + 6, y + offset);
  window.draw(tN2);
  offset += spacing;

  sf::Text tN3;
  tN3.setFont(font);
  tN3.setCharacterSize(textSize);
  tN3.setFillColor(sf::Color::White);
  tN3.setString("Nivel 3: " + std::to_string(costoMejoras[2]));
  tN3.setPosition(x + 6, y + offset);
  window.draw(tN3);
  offset += spacing;

  sf::Text tN4;
  tN4.setFont(font);
  tN4.setCharacterSize(textSize);
  tN4.setFillColor(sf::Color::White);
  tN4.setString("Nivel 4: " + std::to_string(costoMejoras[3]));
  tN4.setPosition(x + 6, y + offset);
  window.draw(tN4);
}

// ==================== GAME OVER ====================

void RenderizadorMapa::activarGameOver()
{
  modoGameOver = true;
  gameOverAlpha = 0.f;

  float ancho = 400.f;
  float x = (window.getSize().x - ancho) / 2.f;
  float centroX = x + ancho / 2.f;

  float yBase = (float)window.getSize().y / 2.f;

  btnReiniciar = std::make_unique<UIButton>(
      font,
      "Reiniciar",
      sf::Vector2f(centroX - 90.f, yBase + 20.f),
      sf::Vector2f(180.f, 40.f));

  btnSalir = std::make_unique<UIButton>(
      font,
      "Salir",
      sf::Vector2f(centroX - 90.f, yBase + 65.f),
      sf::Vector2f(180.f, 40.f));

  btnReiniciar->onClick = [this]()
  {
    std::cout << ">>> Reiniciar juego\n";
    modoGameOver = false;
    gameOverAlpha = 0.f;
  };

  btnSalir->onClick = [this]()
  {
    std::cout << ">>> Salir del juego\n";
    window.close();
  };
}

void RenderizadorMapa::activarSeleccionMaquina()
{
  modoSeleccionMaquina = true;
  seleccionAlpha = 0.f;
  tipoSeleccionado = -1;

  float ancho = 400.f;
  float x = (window.getSize().x - ancho) / 2.f;
  float centroX = x + ancho / 2.f;
  float yBase = (float)window.getSize().y / 2.f;

  btnBFS = std::make_unique<UIButton>(
      font,
      "BFS",
      sf::Vector2f(centroX - 160.f, yBase + 1.f),
      sf::Vector2f(130.f, 50.f));

  btnDFS = std::make_unique<UIButton>(
      font,
      "DFS",
      sf::Vector2f(centroX + 40.f, yBase + 1.f),
      sf::Vector2f(130.f, 50.f));

  btnCancelar = std::make_unique<UIButton>(
      font,
      "Cancelar",
      sf::Vector2f(centroX - 90.f, yBase + 60.f),
      sf::Vector2f(180.f, 40.f));

  // ASIGNAR CALLBACKS
  btnBFS->onClick = [this]()
  {
    tipoSeleccionado = 1;
    modoSeleccionMaquina = false;
    std::cout << ">>> Seleccion BFS\n";
  };

  btnDFS->onClick = [this]()
  {
    tipoSeleccionado = 2;
    modoSeleccionMaquina = false;
    std::cout << ">>> Seleccion DFS\n";
  };

  btnCancelar->onClick = [this]()
  {
    tipoSeleccionado = -1;
    modoSeleccionMaquina = false;
    std::cout << ">>> Cancelado\n";
  };
}

void RenderizadorMapa::dibujarGameOverOverlay()
{
  // Animación sencilla de fade-in
  if (gameOverAlpha < 200.f)
    gameOverAlpha += 4.f;
  if (gameOverAlpha > 200.f)
    gameOverAlpha = 200.f;

  sf::Uint8 alpha = static_cast<sf::Uint8>(gameOverAlpha);

  float ancho = 400.f;
  float alto = 220.f;

  float x = (window.getSize().x - ancho) / 2.f;
  float y = (window.getSize().y - alto) / 2.f;
  float centroX = x + ancho / 2.f;

  // Fondo
  sf::RectangleShape panel(sf::Vector2f(ancho, alto));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(120, 0, 0, alpha));
  panel.setOutlineColor(sf::Color(255, 80, 80, alpha));
  panel.setOutlineThickness(3.f);
  window.draw(panel);

  // Título
  sf::Text titulo("GAME OVER", font, 32);
  titulo.setFillColor(sf::Color(255, 200, 200, alpha));
  sf::FloatRect tb = titulo.getLocalBounds();
  titulo.setOrigin(tb.width / 2.f, tb.height / 2.f);
  titulo.setPosition(centroX, y + 40);
  window.draw(titulo);

  // Mensaje dinámico
  sf::Text msg(mensajeGameOver, font, 18);
  msg.setFillColor(sf::Color(255, 255, 255, alpha));
  sf::FloatRect mb = msg.getLocalBounds();
  msg.setOrigin(mb.width / 2.f, mb.height / 2.f);
  msg.setPosition(centroX, y + 90);
  window.draw(msg);
}

void RenderizadorMapa::dibujarSeleccionMaquinaOverlay()
{
  if (seleccionAlpha < 200.f)
    seleccionAlpha += 4.f;
  if (seleccionAlpha > 200.f)
    seleccionAlpha = 200.f;

  sf::Uint8 alpha = (sf::Uint8)seleccionAlpha;

  float ancho = 400.f;
  float alto = 220.f;

  float x = (window.getSize().x - ancho) / 2.f;
  float y = (window.getSize().y - alto) / 2.f;
  float centroX = x + ancho / 2.f;

  // Fondo
  sf::RectangleShape panel(sf::Vector2f(ancho, alto));
  panel.setPosition(x, y);
  panel.setFillColor(sf::Color(0, 60, 120, alpha));
  panel.setOutlineColor(sf::Color(80, 150, 255, alpha));
  panel.setOutlineThickness(3.f);
  window.draw(panel);

  // Título
  sf::Text titulo("Selecciona el tipo de maquina", font, 26);
  titulo.setFillColor(sf::Color(200, 220, 255, alpha));
  sf::FloatRect tb = titulo.getLocalBounds();
  titulo.setOrigin(tb.width / 2.f, tb.height / 2.f);
  titulo.setPosition(centroX, y + 40);
  window.draw(titulo);
}

void RenderizadorMapa::updateUI()
{
  if (modoGameOver)
  {
    if (btnReiniciar)
      btnReiniciar->update(window);
    if (btnSalir)
      btnSalir->update(window);
  }

  // -------------------
  // MODO SELECCIÓN
  if (modoSeleccionMaquina)
  {
    if (btnBFS)
      btnBFS->update(window);
    if (btnDFS)
      btnDFS->update(window);
    if (btnCancelar)
      btnCancelar->update(window);
    return; // Bloquea botones normales
  }
  if (mostrarBotonesMaquinas && !modoGameOver)
  {
    if (btnConstruir)
      btnConstruir->update(window);
    if (btnMejorar)
      btnMejorar->update(window);
  }
}

void RenderizadorMapa::drawUI()
{
  if (modoGameOver)
  {
    dibujarGameOverOverlay();
    if (btnReiniciar)
      btnReiniciar->draw(window);
    if (btnSalir)
      btnSalir->draw(window);
    return;
  }

  // ---------------------
  // MODO SELECCIÓN
  if (modoSeleccionMaquina)
  {
    dibujarSeleccionMaquinaOverlay();

    if (btnBFS)
      btnBFS->draw(window);
    if (btnDFS)
      btnDFS->draw(window);
    if (btnCancelar)
      btnCancelar->draw(window);
    return;
  }

  // Modo normal
  if (mostrarBotonesMaquinas)
  {
    if (btnConstruir)
      btnConstruir->draw(window);
    if (btnMejorar)
      btnMejorar->draw(window);
  }
}

bool RenderizadorMapa::tieneMaquina(int nodo) const
{
  return maquinas.find(nodo) != maquinas.end();
}

int RenderizadorMapa::nivelMaquina(int nodo) const
{
  auto it = maquinas.find(nodo);
  if (it == maquinas.end())
    return 0;
  return it->second.getNivel();
}
