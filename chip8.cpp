#include "chip8.h"
#include <iostream>
#include <vector>

Chip8::Chip8()
    : program_counter{hardware::memory_program_start}, opcode{0},
      index_register{0}, stack_pointer{0}, memory(hardware::memory_capacity, 0),
      registers(hardware::max_registers, 0),
      display(hardware::display_width,
              std::vector<bool>(hardware::display_height, 0)) {
  for (int i = 0; i < hardware::font_capacity; ++i) {
    memory[i + hardware::memory_font_offset] = hardware::fontset[i];
  }
}

void Chip8::emulateCycle() {
  opcode = memory[program_counter] << 8 |
           memory[program_counter + 1]; // merge both bytes into opcode

  switch (opcode & 0xF000) { // reading the first 4 bits
  case 0x0000: // 0x00E0 and 0x00EE both start with 0x0, so can't rely on the
               // first 4 bits
    switch (opcode & 0x000F) {
    case 0x0000: // TODO 0x00E0 clears the screen
      for (int i = 0; i < hardware::display_width; ++i)
        for (int j = 0; j < hardware::display_height; ++j)
          display[i][j] = 0;
      break;
    case 0x000E: // TODO 0x00EE returns from subroutine
      break;
    }
    break;
  case 0x1000: // TODO 0x1NNN - jump
    break;
  case 0x6000: // 0x6XNN - set register VX to NN
    registers[opcode & 0x0F00] = opcode & 0x00FF;
    break;
  case 0x7000: // 0x7XNN - add NN to register VX
    registers[opcode & 0x0F00] += opcode & 0x00FF;
    break;
  case 0xD000: // 0xDXYN - draw a sprite at (VX, VY) that's 8 pixes wide and N
               // pixels high
  {
    // modulo because the starting position of the sprite should wrap
    int x{registers[((opcode & 0x0F00) >> 8) % hardware::display_width]};
    int y{registers[((opcode & 0x00F0) >> 4) % hardware::display_height]};
    int height{(opcode & 0x000F)};
    constexpr int width{8};

    registers[0xF] = 0;

    break;
  }
  default:
    std::cout << "Wrong opcode: " << opcode << '\n';
  }

  if (delay_timer > 0)
    --delay_timer;

  if (sound_timer > 0) {
    if (sound_timer == 1) {
      std::cout << "beep\n";
    }
    --sound_timer;
  }
}
