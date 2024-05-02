#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

void testMemory(const std::vector<std::uint8_t> &memory) {
  for (auto &a : memory) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(a) << ' ';
  }

  std::cout << '\n';
}

void testDisplay(const std::vector<std::vector<bool>> &vec) {
  for (const auto &row : vec) {
    for (bool value : row) {
      std::cout << (value ? "1" : "0") << " "; // Print 1 for true, 0 for false
    }
    std::cout << '\n'; // Move to the next line after printing each row
  }
  std::cout << '\n';
}
