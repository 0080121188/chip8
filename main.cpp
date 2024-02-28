#include "chip8.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

int main() {
  Chip8 chip8{};

  sf::RenderWindow window(
      sf::VideoMode(hardware::display_width, hardware::display_height),
      "chip8");

  window.clear(sf::Color::Black);
  // sf::CircleShape shape(100.f);
  // shape.setFillColor(sf::Color::Green);

  // Create an image
  sf::Image image;
  image.create(1, 1, sf::Color::White); // Create a 1x1 image with a white pixel

  // Create a texture from the image
  sf::Texture texture;
  texture.loadFromImage(image);

  // Create a sprite to draw the texture
  sf::Sprite sprite(texture);
  sprite.setPosition(10, 10);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
