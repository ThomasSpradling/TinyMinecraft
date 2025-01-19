#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"

namespace Application {

typedef struct Button {
  bool pressed = false;
  bool down = false;
} ButtonState;

typedef struct Keyboard {
  Button keys[GLFW_KEY_LAST];
} Keyboard;

typedef struct Mouse {
  Button buttons[GLFW_MOUSE_BUTTON_LAST];
  glm::vec2 delta, position;
  bool grabbed = false;
  float offsetY = 0.0f;
} Mouse;

class InputHandler {
public:
  Keyboard keyboard;
  Mouse mouse;

  InputHandler() = default;
  ~InputHandler() = default;

  void static KeyCallback(GLFWwindow *handle, int key, int, int action, int);
  void static CursorCallback(GLFWwindow *handle, double x, double y);
  void static MouseCallback(GLFWwindow *handle, int button, int action, int);
  void static ScrollCallback(GLFWwindow *handle, double, double offsetY);

  //// Keyboard input

  // Checks if key was presssed one time.
  bool IsKeyPressed(int key);

  // Checks if key is currently held down.
  bool IsKeyUp(int key);

  // Checks if key is currently up.
  bool IsKeyDown(int key);

  //// Mouse input

  bool IsMouseButtonPressed(int button);

  bool IsMouseButtonUp(int button);

  bool IsMouseButtonDown(int button);

  glm::vec2 GetMousePosition();
  int GetMousePositionX();
  int GetMousePositionY();

  float GetMouseDeltaX();
  float GetMouseDeltaY();

  float GetMouseWheelMove();

private:
  // const float mouseSensitivity = 0.3f;
};

}

#endif // INPUT_HANDLER_H_
