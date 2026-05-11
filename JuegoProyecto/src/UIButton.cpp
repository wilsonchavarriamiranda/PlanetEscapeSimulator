#include "UIButton.h"

UIButton::UIButton(const sf::Font &font,
                   const std::string &texto,
                   sf::Vector2f pos,
                   sf::Vector2f size,
                   sf::Color normal,
                   sf::Color hover,
                   sf::Color press)
{
  colorNormal = normal;
  colorHover = hover;
  colorPressed = press;

  shape.setPosition(pos);
  shape.setSize(size);
  shape.setFillColor(colorNormal);

  label.setFont(font);
  label.setString(texto);
  label.setCharacterSize(18);
  label.setFillColor(sf::Color::White);

  sf::FloatRect b = label.getLocalBounds();
  label.setOrigin(b.width / 2, b.height / 2);
  label.setPosition(pos.x + size.x / 2, pos.y + size.y / 2);
}

bool UIButton::contains(sf::Vector2f punto) const
{
  return shape.getGlobalBounds().contains(punto);
}

void UIButton::update(const sf::RenderWindow &win)
{
  sf::Vector2i mouse = sf::Mouse::getPosition(win);
  sf::Vector2f mp(mouse.x, mouse.y);

  hovered = contains(mp);

  if (!hovered)
  {
    shape.setFillColor(colorNormal);
    return;
  }

  // Hover
  shape.setFillColor(colorHover);

  // Click logico
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
  {
    if (!pressed) // solo dispara 1 vez
    {
      pressed = true;
      shape.setFillColor(colorPressed);

      if (onClick)
        onClick();
    }
  }
  else
  {
    pressed = false;
  }
}

void UIButton::draw(sf::RenderWindow &win)
{
  win.draw(shape);
  win.draw(label);
}
