#include "chip8.h"
#include <iostream>
#include <vector>

Chip8::Chip8()
    : program_counter{hardware::memory_program_start}, opcode{0},
      index_register{0}, stack_pointer{0}, memory(hardware::memory_capacity, 0),
      general_registers(static_cast<int>(registers::MAX_REGISTERS), 0),
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
      break;
    case 0x000E: // TODO 0x00EE returns from subroutine
      break;
    }
  case 0x1000: // TODO 0x1NNN - jump
    break;
  case 0x6000: // 0x6XNN - set register VX to NN
    general_registers[opcode & 0x0F00] = opcode & 0x00FF;
    break;
  case 0x7000: // 0x7XNN - add NN to register VX
    general_registers[opcode & 0x0F00] += opcode & 0x00FF;
  default:
    std::cout << "Wrong opcode: " << opcode << '\n';
  }
}
