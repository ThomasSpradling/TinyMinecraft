#include "Application/InputHandler.h"

namespace Application {

void InputHandler::KeyCallback(GLFWwindow *handle, int key, int, int action, int) {
  if (key < 0) return;

  InputHandler *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
  if (inputHandler == nullptr) return;

  if (action == GLFW_PRESS) {
    inputHandler->keyboard.keys[key].pressed = true;
    inputHandler->keyboard.keys[key].down = true;
  }

  if (action == GLFW_RELEASE) {
    inputHandler->keyboard.keys[key].down = false;
  }
}

void InputHandler::CursorCallback(GLFWwindow *handle, double x, double y) {
  InputHandler *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
  if (inputHandler == nullptr) return;

  if (!inputHandler->mouse.grabbed) {
    inputHandler->mouse.position = { x, y };
    inputHandler->mouse.grabbed = true;
  }

  glm::vec2 curr { x, y };
  inputHandler->mouse.delta = curr - inputHandler->mouse.position;
  inputHandler->mouse.position = curr;

  const float sensitivity = 0.1f;
  inputHandler->mouse.delta *= sensitivity;
}

void InputHandler::MouseCallback(GLFWwindow *handle, int button, int action, int) {
  if (button < 0) return;

  InputHandler *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
  if (inputHandler == nullptr) return;

  if (action == GLFW_PRESS) {
    inputHandler->mouse.buttons[button].pressed = true;
    inputHandler->mouse.buttons[button].down = true;
  }

  if (action == GLFW_RELEASE) {
    inputHandler->mouse.buttons[button].down = false;
  }

}

void InputHandler::ScrollCallback(GLFWwindow *handle, double, double offsetY) {
  InputHandler *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
  if (inputHandler == nullptr) return;

  inputHandler->mouse.offsetY = offsetY;
}


bool InputHandler::IsKeyPressed(int key) {
  bool isPressed = keyboard.keys[key].pressed;
  keyboard.keys[key].pressed = false;

  return isPressed;
}

bool InputHandler::IsKeyUp(int key) {
  return !keyboard.keys[key].down;
}

bool InputHandler::IsKeyDown(int key) {
  return keyboard.keys[key].down;
}

bool InputHandler::IsMouseButtonPressed(int button) {
  bool isPressed = mouse.buttons[button].pressed;
  mouse.buttons[button].pressed = false;

  return isPressed;
}

bool InputHandler::IsMouseButtonUp(int button) {
  return !mouse.buttons[button].down;
}

bool InputHandler::IsMouseButtonDown(int button) {
  return mouse.buttons[button].down;
}

glm::vec2 InputHandler::GetMousePosition() {
  return mouse.position;
}

int InputHandler::GetMousePositionX() {
  return mouse.position.x;
}

int InputHandler::GetMousePositionY() {
  return mouse.position.x;
}

float InputHandler::GetMouseDeltaX() {
  float delta = mouse.delta.x;
  mouse.delta.x = 0.0f;
  return delta;
}

float InputHandler::GetMouseDeltaY() {
 float delta = mouse.delta.y;
  mouse.delta.y = 0.0f;
  return delta;
}

float InputHandler::GetMouseWheelMove() {
  return mouse.offsetY;
}

// void InputHandler::UpdateMouse() {
//   mouse.delta *= mouseSensitivity;
// }

}
