#ifndef WINDOW_H_
#define WINDOW_H_

#include "gfx.h"
#include <iostream>
#include <unordered_map>

typedef struct KeyState {
  bool pressed;
  bool isDown;
} KeyState;

class Window {
public:
  GLFWwindow *handle;
  std::unordered_map<int, KeyState> keyboard;

  Window();
  ~Window();

  void Init(int width, int height, const std::string &name);

  // Checks if key was presssed one time.
  auto IsKeyPressed(int key) -> bool;

  // Checks that key is currently held down.
  auto IsKeyDown(int key) -> bool;

  // Checks that key is currently held up.
  auto IsKeyUp(int key) -> bool;

  auto ShouldClose() -> bool;

  void Close();

  int GetWidth();
  int GetHeight();

  void SwapBuffers();

private:
  int width, height;

  void static ErrorCallback(int code, const char *description) {
    std::cerr << "GLFW error " << code << ": " << description << std::endl;
  }

  void static KeyCallback(GLFWwindow *handle, int key, int, int action, int) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(handle));

    if (action == GLFW_PRESS) {
      window->keyboard[key].pressed = true;
      window->keyboard[key].isDown = true;
    }

    if (action == GLFW_RELEASE) {
      window->keyboard[key].isDown = false;
    }
  }
};

#endif // WINDOW_H_