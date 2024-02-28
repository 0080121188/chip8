#pragma once

#include <array>
#include <cstdint>
#include <vector>

enum class registers {
  V0,
  V1,
  V2,
  V3,
  V4,
  V5,
  V6,
  V7,
  V8,
  V9,
  VA,
  VB,
  VC,
  VD,
  VE,
  VF,            // flag register
  MAX_REGISTERS, // 16
};

enum class display {
  black,
  white,
};

namespace hardware {
inline constexpr int memory_capacity{4096};
inline constexpr int display_width{64};
inline constexpr int display_height{32};
} // namespace hardware

class Chip8 {
private:
  std::vector<std::uint8_t> memory;
  std::vector<std::uint8_t> general_registers;
  std::vector<std::vector<bool>> display;
  std::vector<std::uint16_t> stack{}; // original chip8 has 16 two-byte entries
  std::uint16_t stack_pointer{};
  std::uint16_t program_counter{};
  std::uint16_t index_register{}; // used to point at locations in memory
  std::uint8_t delay_timer{};     // decremented at 60hz until 0
  std::uint8_t sound_timer{};     // like delay timer, beeps if it's not 0
  std::uint16_t opcode{};

public:
  Chip8();

  void emulateCycle();
};
