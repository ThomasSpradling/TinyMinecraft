#ifndef WINDOW_H_
#define WINDOW_H_

#include "Application/InputHandler.h"
#include "Graphics/gfx.h"
#include <iostream>
#include <unordered_map>

namespace Application {

class Window {
public:
  GLFWwindow *handle;

  Window() = default;
  ~Window() = default;

  // Sets up GLFW and attaches the inputHandler to it
  void Initialize(int width, int height, const std::string &name, InputHandler &inputHandler);

  bool ShouldClose();

  void PollEvents();

  void Close();

  int GetWidth();
  int GetHeight();

  void SwapBuffers();

private:
  int width, height;

  void static ErrorCallback(int code, const char *description) {
    std::cerr << "GLFW error " << code << ": " << description << std::endl;
  }
};

}


#endif // WINDOW_H_