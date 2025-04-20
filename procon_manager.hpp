#include <SDL3/SDL.h>
#include <optional>
#include <print>

struct ProconManager {
  std::optional<SDL_Gamepad *> gamepad;
  int num_data = 3;
  float accel[3];
  float gyro[3];
  uint64_t last_updated;

  auto scan() -> bool;
  auto connected() -> bool;
  auto update() -> bool;
  auto print_info() -> void;
  auto print_data() -> void;
  static auto is_procon(SDL_JoystickID instance_id) -> bool;
  static auto print_gamepad(SDL_JoystickID instance_id) -> void;
  static auto print_gamepad(SDL_Gamepad *gamepad) -> void;

  ~ProconManager();

private:
  auto close() -> void;
  auto enable_sensors() -> void;
};

ProconManager::~ProconManager() { this->close(); }

auto ProconManager::close() -> void {
  // close only when we have a gamepad otherwise we will do nothing
  if (this->gamepad.has_value()) {
    SDL_CloseGamepad(this->gamepad.value());
  }
}

auto ProconManager::scan() -> bool {
  auto has_procon = false;

  if (!SDL_HasGamepad()) {
    std::println("Gamepad not found.");
    return false;
  }

  auto gamepads = SDL_GetGamepads(NULL);
  if (gamepads == NULL) {
    std::println("Gamepad not found.");
    return false;
  }

  for (int i = 0; gamepads[i] != 0; i++) {
    // check gamepads[i] is procon
    auto instance_id = gamepads[i];
    if (this->is_procon(instance_id)) {
      // close if we have a gamepad
      this->close();
      this->gamepad = SDL_OpenGamepad(instance_id);
      this->enable_sensors();
      this->update();
      has_procon = true;
      break;
    }
  }

  SDL_free(gamepads);

  return has_procon;
}

auto ProconManager::enable_sensors() -> void {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return;
  }

  SDL_SensorType sensors[] = {
      SDL_SENSOR_ACCEL,
      SDL_SENSOR_GYRO,
  };
  for (SDL_SensorType t : sensors) {
    if (SDL_GamepadHasSensor(this->gamepad.value(), t)) {
      SDL_SetGamepadSensorEnabled(this->gamepad.value(), t, true);
    }
  }
}

auto ProconManager::connected() -> bool {
  if (this->gamepad.has_value()) {
    return SDL_GamepadConnected(this->gamepad.value());
  }

  return false;
}

auto ProconManager::update() -> bool {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return false;
  }

  SDL_UpdateGamepads();

  auto ac_updated = SDL_GetGamepadSensorData(
      this->gamepad.value(), SDL_SENSOR_ACCEL, this->accel, this->num_data);
  auto gy_updated = SDL_GetGamepadSensorData(
      this->gamepad.value(), SDL_SENSOR_GYRO, this->gyro, this->num_data);

  auto updated = ac_updated && gy_updated;

  if (updated) {
    this->last_updated = SDL_GetTicks();
  }

  return updated;
}

auto ProconManager::print_info() -> void {
  if (this->gamepad.has_value()) {
    this->print_gamepad(this->gamepad.value());
  }
}

auto ProconManager::print_data() -> void {
  if (!this->gamepad.has_value()) {
    return;
  }

  std::println("Time:  {}", this->last_updated);
  std::println("Accel: {}, {}, {}", this->accel[0], this->accel[1],
               this->accel[2]);
  std::println("Gyro:  {}, {}, {}", this->gyro[0], this->gyro[1],
               this->gyro[2]);
}

auto ProconManager::is_procon(SDL_JoystickID instance_id) -> bool {
  auto type = SDL_GetGamepadTypeForID(instance_id);

  return type == SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
}

auto ProconManager::print_gamepad(SDL_Gamepad *gamepad) -> void {
  auto instance_id = SDL_GetGamepadID(gamepad);
  print_gamepad(instance_id);
}

auto ProconManager::print_gamepad(SDL_JoystickID instance_id) -> void {
  auto mapping = SDL_GetGamepadMappingForID(instance_id);
  auto name = SDL_GetGamepadNameForID(instance_id);
  auto path = SDL_GetGamepadPathForID(instance_id);
  auto type = SDL_GetGamepadTypeForID(instance_id);
  auto type_str = SDL_GetGamepadStringForType(type);
  auto vendor_id = SDL_GetGamepadVendorForID(instance_id);
  auto product_id = SDL_GetGamepadProductForID(instance_id);
  auto product_ver = SDL_GetGamepadProductVersionForID(instance_id);

  std::println("Mapping: {}", mapping);
  std::println("Device Index: {}", instance_id);
  std::println("Name: {}", name);
  std::println("Path: {}", path);
  std::println("Type: {}", type_str);
  std::println("USB Vendor ID: {}", vendor_id);
  std::println("USB Product ID: {}", product_id);
  std::println("Product Version: {}", product_ver);

  SDL_free(mapping);
}

