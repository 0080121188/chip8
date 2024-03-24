#pragma once

#include <array>
#include <cstdint>
#include <vector>

enum class display {
  black,
  white,
};

namespace hardware {
inline constexpr int max_registers{16};
inline constexpr int memory_capacity{4096};
inline constexpr int memory_program_start{
    0x200}; // the program should start at 0x200 in memory
inline constexpr int display_width{64};
inline constexpr int display_height{32};
inline constexpr int pixel_size{1};
inline constexpr int memory_font_offset{
    0x50}; // the fonts should start at 0x50 in memory
inline constexpr int font_capacity{80}; // how much space the fontset takes up
inline constexpr std::array<std::uint8_t, font_capacity> fontset{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
} // namespace hardware

class Chip8 {
private:
  std::vector<std::vector<bool>> display; // 64x32
  std::vector<std::uint16_t> stack{};  // original chip8 has 16 two-byte entries
  std::vector<std::uint8_t> memory;    // 4096 bytes
  std::vector<std::uint8_t> registers; // 16 general registers
  std::uint16_t opcode{};
  std::uint16_t stack_pointer{};
  std::uint16_t program_counter{};
  std::uint16_t index_register{}; // used to point at locations in memory
  std::uint8_t delay_timer{};     // decremented at 60hz until 0
  std::uint8_t sound_timer{};     // like delay timer, beeps if it's not 0
  bool draw_flag{};

public:
  Chip8();
  void emulateCycle();
  bool getDrawFlag();
  const std::vector<std::vector<bool>> &getDisplay() const;
};
