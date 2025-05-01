#pragma once

#include <SDL3/SDL.h>
#include <optional>
#include <print>
#include <utility>
#include <vector>

struct ProconManager {
  std::optional<SDL_Gamepad *> gamepad;
  int num_data = 3;
  float accel[3];
  float gyro[3];
  uint64_t last_updated;
  uint64_t delta_time;

  auto scan() -> bool;
  auto connected() -> bool;
  auto update() -> bool;

  auto is_button_pressed(SDL_GamepadButton button) -> bool;
  auto is_zbutton_pressed(SDL_GamepadAxis axis) -> bool;
  auto get_axis(SDL_GamepadAxis axis) -> int16_t;

  auto print_info() -> void;
  auto print_data() -> void;
  auto print_sensor() -> void;
  auto print_button() -> void;
  auto print_axis() -> void;

  static auto is_procon(SDL_JoystickID instance_id) -> bool;
  static auto print_gamepad(SDL_JoystickID instance_id) -> void;
  static auto print_gamepad(SDL_Gamepad *gamepad) -> void;

  ~ProconManager();

private:
  auto close() -> void;
  auto enable_sensors() -> void;
};

inline ProconManager::~ProconManager() { this->close(); }

inline auto ProconManager::close() -> void {
  // close only when we have a gamepad otherwise we will do nothing
  if (this->gamepad.has_value()) {
    SDL_CloseGamepad(this->gamepad.value());
  }
}

inline auto ProconManager::scan() -> bool {
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

inline auto ProconManager::enable_sensors() -> void {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return;
  }

  SDL_SensorType sensors[] = {
      SDL_SENSOR_ACCEL,
      SDL_SENSOR_GYRO,
  };
  for (auto t : sensors) {
    if (SDL_GamepadHasSensor(this->gamepad.value(), t)) {
      SDL_SetGamepadSensorEnabled(this->gamepad.value(), t, true);
    }
  }
}

inline auto ProconManager::connected() -> bool {
  if (this->gamepad.has_value()) {
    return SDL_GamepadConnected(this->gamepad.value());
  }

  return false;
}

inline auto ProconManager::update() -> bool {
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
    auto now = SDL_GetTicks();
    this->delta_time = now - this->last_updated;
    this->last_updated = now;
  }

  return updated;
}

inline auto ProconManager::is_button_pressed(SDL_GamepadButton button) -> bool {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return false;
  }

  return SDL_GetGamepadButton(this->gamepad.value(), button);
}

inline auto ProconManager::is_zbutton_pressed(SDL_GamepadAxis axis) -> bool {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return false;
  }

  auto pressed = 32767;
  return SDL_GetGamepadAxis(this->gamepad.value(), axis) == pressed;
}

inline auto ProconManager::get_axis(SDL_GamepadAxis axis) -> int16_t {
  if (!this->gamepad.has_value()) {
    std::println("Gamepad not configured.");
    return 0;
  }

  return SDL_GetGamepadAxis(this->gamepad.value(), axis);
}

inline auto ProconManager::print_info() -> void {
  if (this->gamepad.has_value()) {
    this->print_gamepad(this->gamepad.value());
  }
}

inline auto ProconManager::print_data() -> void {
  if (!this->gamepad.has_value()) {
    return;
  }

  this->print_sensor();
  this->print_button();
  this->print_axis();
}

inline auto ProconManager::print_sensor() -> void {
  std::println("Time Delta:  {}", this->delta_time / 1000.0);
  std::println("Accel: {}, {}, {}", this->accel[0], this->accel[1],
               this->accel[2]);
  std::println("Gyro:  {}, {}, {}", this->gyro[0], this->gyro[1],
               this->gyro[2]);
}

inline auto ProconManager::print_button() -> void {

  auto button = std::vector<std::pair<SDL_GamepadButton, std::string>>{
      {SDL_GAMEPAD_BUTTON_DPAD_UP, "UP"},
      {SDL_GAMEPAD_BUTTON_DPAD_DOWN, "DOWN"},
      {SDL_GAMEPAD_BUTTON_DPAD_LEFT, "LEFT"},
      {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "RIGHT"},
      {SDL_GAMEPAD_BUTTON_EAST, "A"},
      {SDL_GAMEPAD_BUTTON_SOUTH, "B"},
      {SDL_GAMEPAD_BUTTON_NORTH, "X"},
      {SDL_GAMEPAD_BUTTON_WEST, "Y"},
      {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "L"},
      {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "R"},
      {SDL_GAMEPAD_BUTTON_LEFT_STICK, "LEFT_STICK"},
      {SDL_GAMEPAD_BUTTON_RIGHT_STICK, "RIGHT_STICK"},
      {SDL_GAMEPAD_BUTTON_START, "PLUS"},
      {SDL_GAMEPAD_BUTTON_BACK, "MINUS"},
      {SDL_GAMEPAD_BUTTON_GUIDE, "HOME"},
      {SDL_GAMEPAD_BUTTON_MISC1, "CAPTURE"},
  };

  for (auto const &[bt, name] : button) {
    auto pressed = this->is_button_pressed(bt);
    std::println("{}: {}", name, pressed);
  }

  auto trigger = std::vector<std::pair<SDL_GamepadAxis, std::string>>{
      {SDL_GAMEPAD_AXIS_LEFT_TRIGGER, "ZL"},
      {SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, "ZR"},
  };

  for (auto const &[bt, name] : trigger) {
    auto pressed = this->is_zbutton_pressed(bt);
    std::println("{}: {}", name, pressed);
  }
}

inline auto ProconManager::print_axis() -> void {

  auto axis = std::vector<std::pair<SDL_GamepadAxis, std::string>>{
      {SDL_GAMEPAD_AXIS_LEFTX, "LEFT_STICK_X"},
      {SDL_GAMEPAD_AXIS_LEFTY, "LEFT_STICK_Y"},
      {SDL_GAMEPAD_AXIS_RIGHTX, "RIGHT_STICK_X"},
      {SDL_GAMEPAD_AXIS_RIGHTY, "RIGHT_STICK_Y"},
  };

  for (auto const &[ax, name] : axis) {
    auto val = this->get_axis(ax);
    std::println("{}: {}", name, val);
  }
}

inline auto ProconManager::is_procon(SDL_JoystickID instance_id) -> bool {
  auto type = SDL_GetGamepadTypeForID(instance_id);

  return type == SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
}

auto ProconManager::print_gamepad(SDL_Gamepad *gamepad) -> void {
  auto instance_id = SDL_GetGamepadID(gamepad);
  print_gamepad(instance_id);
}

inline auto ProconManager::print_gamepad(SDL_JoystickID instance_id) -> void {
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
