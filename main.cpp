#include "chip8.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {

  try {
    if (argc < 2)
      throw std::invalid_argument("No game loaded");

    std::ifstream rom(argv[1], std::ios::binary);

    if (!rom.is_open())
      throw std::runtime_error("Failed to open the rom file");

    rom.seekg(0, std::ios::end);
    std::streampos rom_size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> memory(hardware::memory_capacity,
                                     0); // 4096 bytes

    for (int i = 0; i < hardware::font_capacity; ++i) {
      memory[i + hardware::memory_font_offset] = hardware::fontset[i];
    }

    rom.read(reinterpret_cast<char *>(memory.data() +
                                      hardware::memory_program_start),
             rom_size);

    std::vector<std::vector<bool>> display(
        hardware::display_width,
        std::vector<bool>(hardware::display_height, 0)); // 64x32
    std::vector<std::uint16_t> stack(
        2, 0); // original chip8 has 16 two-byte entries
    std::vector<std::uint8_t> registers(hardware::max_registers,
                                        0); // 16 general registers
    std::uint16_t opcode{0};
    std::uint16_t stack_pointer{0};
    std::uint16_t program_counter{hardware::memory_program_start};
    std::uint16_t index_register{0}; // used to point at locations in memory
    std::uint8_t delay_timer{};      // decremented at 60hz until 0
    std::uint8_t sound_timer{};      // like delay timer, beeps if it's not 0

    sf::RenderWindow window(
        sf::VideoMode(hardware::display_width * hardware::pixel_size,
                      hardware::display_height * hardware::pixel_size),
        "chip8");
    window.setFramerateLimit(10);
    sf::RectangleShape pixel(
        sf::Vector2f(hardware::pixel_size, hardware::pixel_size));

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
      }

      opcode = memory[program_counter] << 8 |
               memory[program_counter + 1]; // merge both bytes into opcode

      std::cout << "Opcode: " << std::hex << std::setw(4) << std::setfill('0')
                << opcode << '\n';

      switch (opcode & 0xF000) { // reading the first 4 bits
      case 0x0000: // 0x00E0 and 0x00EE both start with 0x0, so can't rely on
                   // the first 4 bits
        switch (opcode & 0x000F) {
        case 0x0000: // 0x00E0 clears the screen
          for (int x = 0; x < hardware::display_width; ++x)
            for (int y = 0; y < hardware::display_height; ++y)
              display[x][y] = 0;
          break;
        case 0x000E: // TODO 0x00EE returns from subroutine
          break;
          // there's also 0NNN which doesn't get used
        }
        break;
      case 0x1000: // 0x1NNN - jump
        program_counter = (opcode & 0x0FFF);
        break;
      case 0x6000: // 0x6XNN - set register VX to NN
        registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        break;
      case 0x7000: // 0x7XNN - add NN to register VX
        registers[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        break;
      case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0000: // 0x8XY0 - set regsiter VX to VY
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
          break;
        case 0x0001: // 0x8XY1 - VX = VX OR VY
          registers[(opcode & 0x0F00) >> 8] =
              registers[(opcode & 0x0F00) >> 8] |
              registers[(opcode & 0x00F0) >> 4];
          break;
        }
      case 0xA000: // 0xANNN - set the index register to NNN
        index_register = (opcode & 0x0FFF);
        break;
      case 0xD000: // 0xDXYN - draw a sprite at (VX, VY) that's 8 pixes wide and
                   // N pixels high
      {
        // modulo because the starting position of the sprite should wrap
        int x{registers[(opcode & 0x0F00) >> 8]};
        int y{registers[(opcode & 0x00F0) >> 4]};
        int height{(opcode & 0x000F)};
        std::uint16_t pixel{};
        constexpr int width{8};

        registers[0xF] = 0;
        for (int yline = 0; yline < height; ++yline) {
          pixel = memory[index_register + yline];
          for (int xline = 0; xline < width; ++xline) {
            if ((pixel & (0x80 >> xline)) != 0) {
              if (display[x + xline][y + yline] == true)
                registers[0xF] = 1;
              display[x + xline][y + yline] =
                  (display[x + xline][y + yline] != true);
            }
          }
        }
      } break;
      default:
        std::cout << "Wrong opcode: " << std::hex << std::setw(4)
                  << std::setfill('0') << opcode << '\n';
      }

      if (delay_timer > 0)
        --delay_timer;

      if (sound_timer > 0) {
        if (sound_timer == 1) {
          std::cout << "beep\n";
        }
        --sound_timer;
      }

      program_counter += 2;

      // testDisplay(display);

      window.clear();

      for (size_t x = 0; x < display.size(); ++x) {
        for (size_t y = 0; y < display[y].size(); ++y) {
          if (display[x][y]) {
            // Create a rectangle shape for each pixel
            pixel.setPosition(x * hardware::pixel_size,
                              y * hardware::pixel_size);
            pixel.setFillColor(sf::Color::White);
            window.draw(pixel);
          }
        }
      }

      window.display();
    }

  } catch (const std::invalid_argument &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (const std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error\n";
    return EXIT_FAILURE;
  }
  return 0;
}
