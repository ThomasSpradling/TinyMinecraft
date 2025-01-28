#ifndef WINDOW_H_
#define WINDOW_H_

#include "Application/InputHandler.h"

namespace Application {

class Window {
public:
  Window(int width, int height, const std::string &name, InputHandler &inputHandler);
  
  ~Window();
  Window(const Window &) = delete;
  auto operator=(const Window &) -> Window & = delete;
  Window(Window &&) = delete;
  auto operator=(Window &&) -> Window & = delete;

  // Sets up GLFW and attaches the inputHandler to it
  // void Initialize(int width, int height, const std::string &name, InputHandler &inputHandler);

  auto ShouldClose() -> bool;

  void PollEvents() const;

  void Close();

  [[nodiscard]] auto GetWidth() const -> int;
  [[nodiscard]] auto GetHeight() const -> int;

  [[nodiscard]] auto GetHandle() const -> GLFWwindow *;

  void SwapBuffers();

private:
  int m_width, m_height;
  GLFWwindow *m_handle;

  void static ErrorCallback(int code, const char *description);
};

}


#endif // WINDOW_H_