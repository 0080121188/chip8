#include "chip8.h"
#include <vector>

// the system expects the program to be loaded at 0x200
// the fontset should be loaded in memory at 0x50 and onwards
// the rest is clearing the memory and the registers
Chip8::Chip8()
    : program_counter{0x200}, opcode{0}, index_register{0}, stack_pointer{0},
      memory(hardware::memory_capacity),
      general_registers(static_cast<int>(registers::MAX_REGISTERS)),
      display(hardware::display_width,
              std::vector<bool>(hardware::display_height, 0)) {
  for (int i = 0; i < hardware::font_capacity; ++i) {
    memory[i + hardware::memory_font_offset] = hardware::fontset[i];
  }
}

void Chip8::emulateCycle() {
  opcode = memory[program_counter] << 8 |
           memory[program_counter + 1]; // merge both bytes into opcode
}
