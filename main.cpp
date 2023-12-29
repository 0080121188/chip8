#include "chip8.h"
#include <SDL2/SDL.h>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

int main() {
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

  std::array<std::uint8_t, chip8::memory_capacity> memory{};
  std::vector<std::uint16_t>
      stack{}; // original chip8 has limited space - 16 two-byte entries
  std::array<std::uint8_t, chip8::register_amount> general_registers{};
  std::uint8_t program_counter{};
  std::uint16_t index_register{}; // used to point at locations in memory
  std::array<std::array<bool, chip8::display_height>, chip8::display_width>
      display{};
  std::uint8_t delay_timer{}; // decremented at 60hz until 0
  std::uint8_t sound_timer{}; // like delay timer, beeps if it's not 0

  // The window we'll be rendering to
  SDL_Window *window = NULL;

  // The surface contained by the window
  SDL_Surface *screenSurface = NULL;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } else {
    // Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, chip8::display_width,
                              chip8::display_height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
      // Get window surface
      screenSurface = SDL_GetWindowSurface(window);

      // Fill the surface white
      SDL_FillRect(screenSurface, NULL,
                   SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

      // Update the surface
      SDL_UpdateWindowSurface(window);

      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (quit == false) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT)
            quit = true;
        }
      }
    }
  }

  // while (true) {
  // fetch
  // decode
  // execute
  // }

  return 0;
}
