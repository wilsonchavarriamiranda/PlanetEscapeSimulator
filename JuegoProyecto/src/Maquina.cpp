#include "Maquina.h"
#include <cmath>

void Maquina::mejorar()
{
  if (nivel < 4)
  {
    nivel++;
  }
}

void Maquina::draw(sf::RenderWindow &window, sf::Vector2f pos, float radioNodo)
{
  sf::Color c = sf::Color::Magenta;

  switch (nivel)
  {
  case 1:
  {
    sf::CircleShape tri(radioNodo * 0.8f, 3);
    tri.setFillColor(c);
    tri.setOrigin(tri.getRadius(), tri.getRadius());
    tri.setPosition(pos);
    window.draw(tri);
  }
  break;

  case 2:
  {
    sf::RectangleShape sq(sf::Vector2f(radioNodo * 1.3f, radioNodo * 1.3f));
    sq.setFillColor(c);
    sq.setOrigin(sq.getSize() / 2.f);
    sq.setPosition(pos);
    window.draw(sq);
  }
  break;

  case 3:
  {
    sf::CircleShape pent(radioNodo * 0.9f, 5);
    pent.setFillColor(c);
    pent.setOrigin(pent.getRadius(), pent.getRadius());
    pent.setPosition(pos);
    window.draw(pent);
  }
  break;
  case 4:
  {
    sf::ConvexShape star;
    const std::size_t points = 10;
    star.setPointCount(points);

    // create a star by alternating outer and inner radii
    for (std::size_t i = 0; i < points; ++i)
    {
      float angle = static_cast<float>(i) * 2.f * 3.14159265358979323846f / static_cast<float>(points);
      float r = (i % 2 == 0) ? radioNodo : radioNodo * 0.45f;
      sf::Vector2f p = {radioNodo + std::cos(angle) * r, radioNodo + std::sin(angle) * r};
      star.setPoint(i, p);
    }

    star.setFillColor(c);
    star.setOrigin(radioNodo, radioNodo);
    star.setPosition(pos);
    window.draw(star);
  }
  break;
  }
}
