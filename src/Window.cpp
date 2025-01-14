#include "Window.h"
#include "GLFW/glfw3.h"
#include "gfx.h"

Window::Window() {
  
}

Window::~Window() {}

void Window::Init(int width, int height, const std::string &name) {
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

  glfwSetWindowUserPointer(handle, this);

  glfwSetKeyCallback(handle, KeyCallback);

  glfwMakeContextCurrent(handle);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cerr << "Error initializing GLAD." << std::endl;
    glfwTerminate();
    exit(1);
  }

  glfwSwapInterval(1);

  std::cout << "Initialized window." << std::endl;
}

auto Window::IsKeyPressed(int key) -> bool {
  bool isPressed = keyboard[key].pressed;
  keyboard[key].pressed = false;

  return isPressed;
}

auto Window::IsKeyDown(int key) -> bool {
  return keyboard[key].isDown;
}

auto Window::IsKeyUp(int key) -> bool {
  return !keyboard[key].isDown;
}

void Window::StartDrawing() {
  int width, height;
  glfwGetFramebufferSize(handle, &width, &height);
  glViewport(0, 0, width, height);
}
  
void Window::StopDrawing() {
  glfwSwapBuffers(handle);
}

auto Window::ShouldClose() -> bool {
  return glfwWindowShouldClose(handle);
}

void Window::Close() {
  std::cout << "Destroying window." << std::endl;

  glfwSetWindowShouldClose(handle, GLFW_TRUE);
}