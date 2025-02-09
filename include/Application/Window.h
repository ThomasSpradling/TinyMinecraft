#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include "Graphics/gfx.h"
#include "Utils/mathgl.h"
#include "Application/InputHandler.h"

namespace TinyMinecraft {

  namespace Application {

    class Window {
    public:
      Window(int width, int height, const std::string &name);
      
      ~Window();
      Window(const Window &) = delete;
      auto operator=(const Window &) -> Window & = delete;
      Window(Window &&) = delete;
      auto operator=(Window &&) -> Window & = delete;

      [[nodiscard]] inline auto ShouldClose() const -> bool { return glfwWindowShouldClose(m_handle); }
      inline void PollEvents() const { glfwPollEvents(); }
      inline void Close() const { glfwSetWindowShouldClose(m_handle, GLFW_TRUE); }

      [[nodiscard]] inline auto GetWidth() const -> int { return m_width; }
      [[nodiscard]] inline auto GetHeight() const -> int { return m_height; }
      inline void SwapBuffers() const { glfwSwapBuffers(m_handle); }

    private:
      int m_width, m_height;
      GLFWwindow *m_handle;

      struct Mouse {
        bool grabbed = false;
        glm::vec2 delta { 0.0f }, position { 0.0f };
      } m_mouse;

      void static KeyCallback(GLFWwindow *handle, int key, int, int action, int);
      void static CursorCallback(GLFWwindow *handle, double x, double y);
      void static MouseButtonCallback(GLFWwindow *handle, int button, int action, int);
      void static ScrollCallback(GLFWwindow *handle, double, double offsetY);
      void static ErrorCallback(int code, const char *description);
    };

  }

}

#endif // WINDOW_H_