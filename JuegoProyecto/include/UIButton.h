#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <SFML/Graphics.hpp>
#include <functional> // std::function

/**
 * @brief Clase que representa un botón de interfaz gráfica en SFML.
 *
 * Soporta estados normales, hover y presionado, y puede ejecutar un callback al hacer click.
 */
class UIButton
{
private:
  /// Forma del botón
  sf::RectangleShape shape;

  /// Texto del botón
  sf::Text label;

  /// Color cuando el botón está en estado normal
  sf::Color colorNormal;

  /// Color cuando el botón está en estado hover
  sf::Color colorHover;

  /// Color cuando el botón está presionado
  sf::Color colorPressed;

  /// Indica si el cursor está sobre el botón
  bool hovered = false;

  /// Indica si el botón está presionado
  bool pressed = false;

public:
  /// Callback que se ejecuta al hacer click en el botón
  std::function<void()> onClick;

  /**
   * @brief Constructor del botón.
   *
   * @param font Fuente del texto.
   * @param texto Texto a mostrar en el botón.
   * @param pos Posición del botón en la ventana.
   * @param size Tamaño del botón.
   * @param normal Color normal del botón (por defecto gris oscuro).
   * @param hover Color cuando el cursor está sobre el botón.
   * @param press Color cuando el botón está presionado.
   */
  UIButton(const sf::Font &font,
           const std::string &texto,
           sf::Vector2f pos,
           sf::Vector2f size,
           sf::Color normal = sf::Color(60, 60, 60),
           sf::Color hover = sf::Color(90, 90, 90),
           sf::Color press = sf::Color(120, 120, 120));

  /**
   * @brief Actualiza el estado del botón según la posición del mouse y clicks.
   * @param win Ventana de SFML donde se interactúa con el botón.
   */
  void update(const sf::RenderWindow &win);

  /**
   * @brief Dibuja el botón en la ventana.
   * @param win Ventana de SFML donde se dibuja el botón.
   */
  void draw(sf::RenderWindow &win);

  /**
   * @brief Verifica si un punto está dentro del botón.
   * @param punto Punto a verificar (coordenadas de la ventana).
   * @return true Si el punto está dentro del botón.
   * @return false Si el punto está fuera del botón.
   */
  bool contains(sf::Vector2f punto) const;
};

#endif // UI_BUTTON_H
