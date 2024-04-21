#include <iostream>
#include <vector>

void testDisplay(const std::vector<std::vector<bool>> &vec) {
  for (const auto &row : vec) {
    for (bool value : row) {
      std::cout << (value ? "1" : "0") << " "; // Print 1 for true, 0 for false
    }
    std::cout << '\n'; // Move to the next line after printing each row
  }
  std::cout << '\n';
}
