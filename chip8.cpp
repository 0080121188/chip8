#include <cstdint>

bool isOverflow(std::uint8_t x, std::uint8_t y) { return (x > UINT8_MAX - y); }
bool isUnderflow(std::uint8_t x, std::uint8_t y) { return (y > x); }
