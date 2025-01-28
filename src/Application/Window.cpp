#include "Application/Window.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"

namespace Application {

Window::Window(int width, int height, const std::string &name, InputHandler &inputHandler)
  : m_width(width)
  , m_height(height)
{
  PROFILE_FUNCTION(Window)

  const int DEPTH_BUFFER_SIZE = 32;

  if (!glfwInit()) {
    Utils::g_logger.Error("GLFW: Error initializing.");
    exit(1);
  }

  glfwSetErrorCallback(ErrorCallback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  
  m_handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  if (m_handle == nullptr) {
    Utils::g_logger.Error("GLFW: Error creating window.");
    exit(1);
  }

  glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetWindowUserPointer(m_handle, &inputHandler);

  glfwSetKeyCallback(m_handle, InputHandler::KeyCallback);
  glfwSetCursorPosCallback(m_handle, InputHandler::CursorCallback);
  glfwSetMouseButtonCallback(m_handle, InputHandler::MouseCallback);
  glfwSetScrollCallback(m_handle, InputHandler::ScrollCallback);

  glfwMakeContextCurrent(m_handle);

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    Utils::g_logger.Error("GLAD: Error initializing.");
    exit(1);
  }

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

  glfwWindowHint(GLFW_DEPTH_BITS, DEPTH_BUFFER_SIZE);

  glfwSwapInterval(0);

  Utils::g_logger.Message("Window created and initialized successfully.");
}


Window::~Window() {
  Utils::g_logger.Message("Destroying window.");

  glfwTerminate();
}

void Window::PollEvents() const {
  glfwPollEvents();
}

auto Window::ShouldClose() -> bool {
  return glfwWindowShouldClose(m_handle);
}

void Window::Close() {
  glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

auto Window::GetWidth() const -> int {
  return m_width;
}

auto Window::GetHeight() const -> int {
  return m_height;
}

auto Window::GetHandle() const -> GLFWwindow * {
  return m_handle;
}

void Window::SwapBuffers() {
  glfwSwapBuffers(m_handle);
}

void Window::ErrorCallback(int code, const char *description) {
  Utils::g_logger.Error("GLFW error {}: {}", code, description);
}

}
