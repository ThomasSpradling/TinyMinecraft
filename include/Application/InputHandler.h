#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "Graphics/gfx.h"
#include <unordered_map>

namespace Application {

typedef struct KeyState {
  bool pressed;
  bool isDown;
} KeyState;

class InputHandler {
public:
  std::unordered_map<int, KeyState> keyboard;

  InputHandler() = default;
  ~InputHandler() = default;

  // Checks if key was presssed one time.
  bool IsKeyPressed(int key);

  // Checks if key is currently held down.
  bool IsKeyUp(int key);

  // Checks if key is currently up.
  bool IsKeyDown(int key);

  void static KeyCallback(GLFWwindow *handle, int key, int, int action, int) {
    InputHandler *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));

    if (action == GLFW_PRESS) {
      inputHandler->keyboard[key].pressed = true;
      inputHandler->keyboard[key].isDown = true;
    }

    if (action == GLFW_RELEASE) {
      inputHandler->keyboard[key].isDown = false;
    }
  }
private:

};

}

#endif // INPUT_HANDLER_H_
