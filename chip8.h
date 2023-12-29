#pragma once

namespace chip8 {
inline constexpr int memory_capacity{4096};
inline constexpr int register_amount{16};
inline constexpr int display_width{64};
inline constexpr int display_height{32};

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
  VF, // flag register
};

enum class display {
  black,
  white,
};

} // namespace chip8
