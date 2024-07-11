#include "chip8.h"
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <utility>

// TODO: change opcode to union, so that accessing the nibbles is easier to read
// read
// TODO: clean up the comments (sometimes they're above the code, sometimes
// they're next to it)

int main(int argc, char *argv[]) {

  try {
    if (argc < 2)
      throw std::invalid_argument("No game loaded");

    int fps{600};
    bool debug_mode{false};

    for (int i = 2; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-d") {
        debug_mode = true;
      } else if (arg == "-fps" && i + 1 < argc) {
        try {
          fps = std::stoi(argv[++i]);
        } catch (const std::exception &e) {
          std::cerr << "Invalid FPS value" << std::endl;
          fps = 600;
        }
      }
    }

    std::ifstream rom(argv[1], std::ios::binary);

    if (!rom.is_open())
      throw std::runtime_error("Failed to open the rom file");

    rom.seekg(0, std::ios::end);
    std::streampos rom_size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> memory(hardware::memory_capacity,
                                     0); // 4096 bytes

    for (int i = 0; i < hardware::font_capacity; ++i) {
      memory[i] = hardware::fontset[i];
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
                                            //    std::uint16_t opcode{0};
    std::uint16_t stack_pointer{0};
    std::uint16_t program_counter{hardware::memory_program_start};
    std::uint16_t index_register{0}; // used to point at locations in memory
    std::uint8_t delay_timer{};      // decremented at 60hz until 0
    std::uint8_t sound_timer{};      // like delay timer, beeps if it's not 0

    // since the keypad is hexadecimal there has to be a qwerty equivalent
    std::map<int, sf::Keyboard::Key> keyboard_map{
        {0x1, sf::Keyboard::Num1}, {0x2, sf::Keyboard::Num2},
        {0x3, sf::Keyboard::Num3}, {0xC, sf::Keyboard::Num4},
        {0x4, sf::Keyboard::Q},    {0x5, sf::Keyboard::W},
        {0x6, sf::Keyboard::E},    {0xD, sf::Keyboard::R},
        {0x7, sf::Keyboard::A},    {0x8, sf::Keyboard::S},
        {0x9, sf::Keyboard::D},    {0xE, sf::Keyboard::F},
        {0xA, sf::Keyboard::Z},    {0x0, sf::Keyboard::X},
        {0xB, sf::Keyboard::C},    {0xF, sf::Keyboard::V}};

    sf::RenderWindow window(
        sf::VideoMode(hardware::display_width * hardware::pixel_size,
                      hardware::display_height * hardware::pixel_size),
        "chip8");
    window.setFramerateLimit(fps);
    sf::RectangleShape pixel(
        sf::Vector2f(hardware::pixel_size, hardware::pixel_size));

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("beep.ogg"))
      throw std::runtime_error("Failed to load the beep sound.");

    sf::Sound sound;
    sound.setBuffer(buffer);

    Opcode opcode{};

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
      }

      opcode.full = memory[program_counter] << 8 |
                    memory[program_counter + 1]; // merge both bytes into opcode

      if (debug_mode)
        std::cout << "Opcode: " << std::hex << std::setw(4) << std::setfill('0')
                  << opcode.full << '\n';

      program_counter += 2;

      switch (opcode.nibble1) { // reading the first 4 bits
      case 0x0:
        switch (opcode.nibble4) {
        case 0x0: // 0x00E0 clears the screen
          for (int x = 0; x < hardware::display_width; ++x)
            for (int y = 0; y < hardware::display_height; ++y)
              display[x][y] = 0;
          break;
        case 0xE: // 0x00EE returns from a call
          program_counter = stack.back();
          stack.pop_back();
          break;
          // there's also 0NNN which doesn't get used
        }
        break;
      case 0x1: // 0x1NNN - jump to NNN
        program_counter = (opcode.full & 0x0FFF);
        break;
      case 0x2: // 0x2NNN - call something at NNN
        stack.push_back(program_counter);
        program_counter = (opcode.full & 0x0FFF);
        break;
      case 0x3: // 0x3XNN - skip one instruction if VX == NN
        if (registers[opcode.nibble2] == (opcode.full & 0x00FF))
          program_counter += 2;
        break;
      case 0x4: // 0x4XNN - skip one instruction if VX != NN
        if (registers[opcode.nibble2] != (opcode.full & 0x00FF))
          program_counter += 2;
        break;
      case 0x5: // 0x5XY0 - skip one instruction if VX == VY
        if (registers[opcode.nibble2] == registers[opcode.nibble3])
          program_counter += 2;
        break;
      case 0x6: // 0x6XNN - set register VX to NN
        registers[opcode.nibble2] = (opcode.full & 0x00FF);
        break;
      case 0x7: // 0x7XNN - add NN to register VX. This instruction doesn't
                // set the carry flag even if it overflows
        registers[opcode.nibble2] += (opcode.full & 0x00FF);
        break;
      case 0x8:
        switch (opcode.nibble4) {
        case 0x0: // 0x8XY0 - set regsiter VX to VY
          registers[opcode.nibble2] = registers[opcode.nibble3];
          break;
        case 0x1: // 0x8XY1 - VX = VX OR VY
          registers[opcode.nibble2] =
              registers[opcode.nibble2] | registers[opcode.nibble3];
          break;
        case 0x2: // 0x8XY2 - VX = VX AND VY
          registers[opcode.nibble2] =
              registers[opcode.nibble2] & registers[opcode.nibble3];
          break;
        case 0x3: // 0x8XY3 - VX = VX XOR VY
          registers[opcode.nibble2] =
              registers[opcode.nibble2] ^ registers[opcode.nibble3];
          break;
        case 0x4: // 0x8XY4 - VX = VX + VY
        {
          // x and y are needed because we have to first do the
          // operation, and then check if it originally
          // overflowed (otherwise using the flag register as
          // one of the operands won't work correctly)
          std::uint8_t x{registers[opcode.nibble2]};
          std::uint8_t y{registers[opcode.nibble3]};
          registers[opcode.nibble2] += registers[opcode.nibble3];
          if (isOverflow(x, y))
            registers[0xF] = 1;
          else
            registers[0xF] = 0;
        } break;
        case 0x5: // 0x8XY5 - VX = VX - VY
        {
          std::uint8_t x{registers[opcode.nibble2]};
          std::uint8_t y{registers[opcode.nibble3]};
          registers[opcode.nibble2] -= registers[opcode.nibble3];
          if (isUnderflow(x, y))
            registers[0xF] = 0;
          else
            registers[0xF] = 1;
        } break;
        case 0x6: // 0x8XY6 - set VX to VY and then shift it to the right
        {
          std::uint8_t x{registers[opcode.nibble2]};
          std::uint8_t y{registers[opcode.nibble3]};
          registers[opcode.nibble2] = registers[opcode.nibble3];
          registers[opcode.nibble2] = (registers[opcode.nibble2] >> 1);
          if (std::bitset<8>{x}.test(0) == true)
            registers[0xF] = 1;
          else
            registers[0xF] = 0;
        } break;
        case 0x7: // 0x8XY7 - VX = VY - VX
        {
          std::uint8_t x{registers[opcode.nibble2]};
          std::uint8_t y{registers[opcode.nibble3]};
          registers[opcode.nibble2] =
              registers[opcode.nibble3] - registers[opcode.nibble2];
          if (isOverflow(x, y))
            registers[0xF] = 0;
          else
            registers[0xF] = 1;
        } break;
        case 0xE: // 0x8XYE - set VX to VY and then shift it to the left
        {
          std::uint8_t x{registers[opcode.nibble2]};
          std::uint8_t y{registers[opcode.nibble3]};
          registers[opcode.nibble2] = registers[opcode.nibble3];
          registers[opcode.nibble2] = (registers[opcode.nibble2] << 1);
          if (std::bitset<8>{x}.test(7) == true)
            registers[0xF] = 1;
          else
            registers[0xF] = 0;
        } break;
        }
        break;
      case 0x9: // 0x9XY0 - skip one instruction if VX != VY
        if (registers[opcode.nibble2] != registers[opcode.nibble3])
          program_counter += 2;
        break;
      case 0xA: // 0xANNN - set the index register to NNN
        index_register = (opcode.full & 0x0FFF);
        break;
      case 0xB: // 0xBNNN - jump to NNN + V0
        program_counter = (opcode.full & 0x0FFF) + registers[0];
        break;
      case 0xC: // 0xCXNN - generate a random number and AND it with NN -
                // put it in VX
        static std::mt19937 mt{std::random_device{}()};
        static std::uniform_int_distribution generator{INT8_MIN, INT8_MAX};
        registers[opcode.nibble2] = (generator(mt) & (opcode.full & 0x00FF));
        break;
      case 0xD: // 0xDXYN - draw a sprite at (VX, VY) that's 8 pixes wide
                // and N pixels high
      {
        // modulo because the starting position of the sprite should wrap
        int x{registers[opcode.nibble2]};
        int y{registers[opcode.nibble3]};
        int height{(opcode.nibble4)};
        std::uint8_t pixel{};
        constexpr int width{8};

        registers[0xF] = 0;
        for (int yline = 0; yline < height; ++yline) {
          pixel = memory[index_register + yline];
          for (int xline = 0; xline < width; ++xline) {
            int wrapped_x = (x + xline) % hardware::display_width;
            int wrapped_y = (y + yline) % hardware::display_height;
            // check if the pixel at position xline is set to 1 (0x80 is
            // 10000000)
            if ((pixel & (0x80 >> xline)) != 0) {
              if (display[wrapped_x][wrapped_y] == true)
                registers[0xF] = 1;
              // monochrome pixel (true of false), so it's gonna flip the state
              // of the pixel at wrapped_x and wrapped_y
              display[wrapped_x][wrapped_y] = !display[wrapped_x][wrapped_y];
            }
          }
        }
      } break;
      case 0xE:
        switch (opcode.nibble3) {
        case 0x9: // 0xEX9E - skip one instruction if the key corresponding
                  // to the value in VX is pressed
          if (sf::Keyboard::isKeyPressed(
                  keyboard_map[registers[opcode.nibble2]]))
            program_counter += 2;
          break;
        case 0xA: // 0xEXA1 - skip one instruction if the key corresponding
                  // to the value in VX is NOT pressed
        {
          auto key = keyboard_map.find(registers[opcode.nibble2]);
          if (!sf::Keyboard::isKeyPressed(key->second))
            program_counter += 2;
        } break;
        }
      case 0xF:
        switch (opcode.nibble4) {
        case 0x3: // 0xFX33 - BCD conversion - take the number in VX (say
                  // 127), convert it to its three digits (so 1, 2, and 7),
                  // and store it in memory (1 at the address in the index
                  // register, 2 at index + 1, and 8 at index + 2)
        {
          std::uint8_t number = registers[opcode.nibble2];
          memory[index_register] = number / 100;
          memory[index_register + 1] = (number / 10) % 10;
          memory[index_register + 2] = number % 10;
        } break;
        case 0x5:
          switch (opcode.nibble3) {
          case 0x1: // 0xFX15 - set the delay timer to the value in VX
            delay_timer = registers[opcode.nibble2];
            break;
          case 0x5: // 0xFX55 - store the value of each register V0 to VX
                    // (INCLUSIVE) in successive memory addresses, starting
                    // with the one stored in the index register (so, V0
                    // will be at index + 0, V1 will be at index + 1, etc.)
          {
            int x = opcode.nibble2;
            for (int i = 0; i <= x; ++i) {
              memory[index_register + i] = registers[i];
            }
            index_register += x + 1; // +1 is here to match the loop condition
          } break;
          case 0x6: // 0xFX65 - the same as 0xFX55 but reverse
          {
            int x = opcode.nibble2;
            for (int i = 0; i <= x; ++i) {
              registers[i] = memory[index_register + i];
            }
            index_register += x + 1; // +1 to match the loop condition
          } break;
          }
          break;
        case 0x7: // 0xFX07 - set register VX to delay timer
          registers[opcode.nibble2] = delay_timer;
          break;
        case 0x8: // 0xFX18 - set the sound timer to the value in VX
          sound_timer = registers[opcode.nibble2];
          break;
        case 0x9: // 0xFX29 - set the index register to the address of the
                  // hex character in VX (they're stored at the beginning
                  // of memory)
        {
          std::uint8_t character = registers[opcode.nibble2];
          index_register =
              0 + character * 5; // 0 because the characters are at the
                                 // beginning of the memory, and 5 because each
                                 // character takes up 5 elements of the array
        } break;
        case 0xA: // 0xFX0A - waits for key input - if a key is pressed,
                  // its hex value is put into XV
        {
          bool was_key_pressed = false;
          for (const auto &key : keyboard_map) {
            if (sf::Keyboard::isKeyPressed(key.second)) {
              registers[opcode.nibble2] = key.first;
              was_key_pressed = true;
              break;
            }
          }
          if (!was_key_pressed)
            program_counter -= 2;
        } break;
        case 0xE: // 0xFX1E - add the value in VX to the index
                  // register
          index_register += registers[opcode.nibble2];
          // overflows outside of the normal addressing range (if it overflows
          // from 0FFF to above 1000)
          if ((index_register & 0xF000) != 0x0000)
            registers[0xF] = 1;
          else
            registers[0xF] = 0;
          break;
        }
        break;
      default:
        if (debug_mode)
          std::cout << "Wrong opcode: " << std::hex << std::setw(4)
                    << std::setfill('0') << opcode.full << '\n';
      }

      if (delay_timer > 0)
        --delay_timer;

      if (sound_timer > 0) {
        if (sound_timer == 1) {
          sound.play();
        }
        --sound_timer;
      }

      window.clear();

      for (size_t x = 0; x < display.size(); ++x) {
        for (size_t y = 0; y < display[y].size(); ++y) {
          if (display[x][y]) {
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
