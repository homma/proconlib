#include "procon_manager.hpp"
#include <SDL3/SDL.h>
#include <memory>

auto init_sdl() -> void {
  if (!SDL_Init(SDL_INIT_GAMEPAD)) {
    exit(1);
  }
}

auto check_procon() -> void {
  auto gp = std::make_unique<ProconManager>();
  if (!gp->scan()) {
    return;
  }

  auto count = 10;
  for (auto i = 0; i < count; i++) {
    std::println("loop {}", i);
    if (!gp->connected()) {
      if (!gp->scan()) {
        return;
      }
    }
    gp->update();
    gp->print_data();
    std::println();

    if (i < count - 1) {
      SDL_Delay(1000);
    }
  }
}

auto main() -> int {
  init_sdl();

  check_procon();
}
