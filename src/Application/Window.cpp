#include "Application/Window.h"
#include "Graphics/gfx.h"
#include "Utils/Profiler.h"

namespace Application {

void Window::Initialize(int initWidth, int initHeight, const std::string &name, InputHandler &inputHandler) {
  PROFILE_FUNCTION(Window)
  
  width = initWidth;
  height = initHeight;

  if (!glfwInit()) {
    std::cerr << "Error initializing GLFW." << std::endl;
    exit(1);
  }

  glfwSetErrorCallback(ErrorCallback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  
  handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  if (handle == nullptr) {
    std::cerr << "Error creating GLFW window." << std::endl;
    glfwTerminate();
    exit(1);
  }

  glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetWindowUserPointer(handle, &inputHandler);

  glfwSetKeyCallback(handle, InputHandler::KeyCallback);
  glfwSetCursorPosCallback(handle, InputHandler::CursorCallback);
  glfwSetMouseButtonCallback(handle, InputHandler::MouseCallback);
  glfwSetScrollCallback(handle, InputHandler::ScrollCallback);

  glfwMakeContextCurrent(handle);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cerr << "Error initializing GLAD." << std::endl;
    glfwTerminate();
    exit(1);
  }

  glfwWindowHint(GLFW_DEPTH_BITS, 32);

  glfwSwapInterval(0);

  std::cout << "Initialized window." << std::endl;
}

void Window::PollEvents() {
  glfwPollEvents();
}

bool Window::ShouldClose() {
  return glfwWindowShouldClose(handle);
}

void Window::Close() {
  std::cout << "Destroying window." << std::endl;

  glfwSetWindowShouldClose(handle, GLFW_TRUE);
}

int Window::GetWidth() {
  return width;
}

int Window::GetHeight() {
  return height;
}

void Window::SwapBuffers() {
  glfwSwapBuffers(handle);
}

}
