#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"
#include <array>

namespace Application {

using ButtonState = struct Button {
  bool pressed = false;
  bool down = false;
};

using Keyboard = struct Keyboard {
  std::array<Button, GLFW_KEY_LAST> keys;
};

using Mouse = struct Mouse {
  std::array<Button, GLFW_MOUSE_BUTTON_LAST> buttons;
  glm::vec2 delta, position;
  bool grabbed = false;
  float offsetY = 0.0f;
};

class InputHandler {
public:
  Keyboard m_keyboard;
  Mouse m_mouse;

  InputHandler() = default;

  void static KeyCallback(GLFWwindow *handle, int key, int, int action, int);
  void static CursorCallback(GLFWwindow *handle, double x, double y);
  void static MouseCallback(GLFWwindow *handle, int button, int action, int);
  void static ScrollCallback(GLFWwindow *handle, double, double offsetY);

  //// Keyboard input

  // Checks if key was presssed one time.
  auto IsKeyPressed(int key) -> bool;

  // Checks if key is currently held down.
  [[nodiscard]] auto IsKeyUp(int key) const -> bool;

  // Checks if key is currently up.
  [[nodiscard]] auto IsKeyDown(int key) const -> bool;

  //// Mouse input

  auto IsMouseButtonPressed(int button) -> bool;

  [[nodiscard]] auto IsMouseButtonUp(int button) const -> bool;

  [[nodiscard]] auto IsMouseButtonDown(int button) const -> bool;

  [[nodiscard]] auto GetMousePosition() const -> glm::vec2;
  [[nodiscard]] auto GetMousePositionX() const -> float;
  [[nodiscard]] auto GetMousePositionY() const -> float;

  auto GetMouseDeltaX() -> float;
  auto GetMouseDeltaY() -> float;

  [[nodiscard]] auto GetMouseWheelMove() const -> float;

private:
  // const float mouseSensitivity = 0.3f;
};

}

#endif // INPUT_HANDLER_H_
