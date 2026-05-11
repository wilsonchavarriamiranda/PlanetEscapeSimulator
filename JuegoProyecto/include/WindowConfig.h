#ifndef WINDOW_CONFIG_H
#define WINDOW_CONFIG_H

#include <SFML/Graphics.hpp>
#include <fstream>

/**
 * @brief Configuración de la ventana de SFML.
 *
 * Permite almacenar dimensiones, posición y modo fullscreen,
 * así como guardar y cargar la configuración desde un archivo.
 */
struct WindowConfig
{
  /// Ancho de la ventana en píxeles
  unsigned int width = 1280;

  /// Alto de la ventana en píxeles
  unsigned int height = 720;

  /// Posición X de la ventana en la pantalla
  int posX = 100;

  /// Posición Y de la ventana en la pantalla
  int posY = 100;

  /// Indica si la ventana es fullscreen
  bool fullscreen = false;

  /**
   * @brief Guarda la configuración de la ventana en un archivo.
   *
   * @param path Ruta del archivo donde se guardará la configuración.
   *
   * Escribe los valores width, height, posX, posY y fullscreen separados por espacios.
   */
  void save(const std::string &path)
  {
    std::ofstream f(path);
    if (!f)
      return;

    f << width << " "
      << height << " "
      << posX << " "
      << posY << " "
      << fullscreen << "\n";
  }

  /**
   * @brief Carga la configuración de la ventana desde un archivo.
   *
   * @param path Ruta del archivo desde donde se cargará la configuración.
   *
   * Lee los valores width, height, posX, posY y fullscreen.
   */
  void load(const std::string &path)
  {
    std::ifstream f(path);
    if (!f)
      return;
    f >> width >> height >> posX >> posY >> fullscreen;
  }
};

#endif // WINDOW_CONFIG_H
