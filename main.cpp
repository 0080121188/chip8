#include "chip8.h"
#include <SDL2/SDL.h>
#include <SFML/Graphics.hpp>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

int main() {

  std::array<std::uint8_t, chip8::memory_capacity> memory{};
  std::vector<std::uint16_t>
      stack{}; // original chip8 has limited space - 16 two-byte entries
  std::array<std::uint8_t, chip8::register_amount> general_registers{};
  std::uint8_t program_counter{};
  std::uint16_t index_register{}; // used to point at locations in memory
  std::array<std::array<bool, chip8::display_height>, chip8::display_width>
      display{};
  std::uint8_t delay_timer{}; // decremented at 60hz until 0
  std::uint8_t sound_timer{}; // like delay timer, beeps if it's not 0

  sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(shape);
    window.display();
  }

  // while (true) {
  // fetch
  // decode
  // execute
  // }

  return 0;
}
