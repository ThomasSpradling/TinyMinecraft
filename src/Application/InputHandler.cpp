#include "Application/InputHandler.h"

namespace TinyMinecraft {

  namespace Application {

    void InputHandler::KeyCallback(GLFWwindow *handle, int key, int, int action, int) {
      if (key < 0) return;

      auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
      if (inputHandler == nullptr) return;

      if (action == GLFW_PRESS) {
        inputHandler->m_keyboard.keys.at(key).pressed = true;
        inputHandler->m_keyboard.keys.at(key).down = true;
      }

      if (action == GLFW_RELEASE) {
        inputHandler->m_keyboard.keys.at(key).down = false;
      }
    }

    void InputHandler::CursorCallback(GLFWwindow *handle, double x, double y) {
      auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
      if (inputHandler == nullptr) return;

      if (!inputHandler->m_mouse.grabbed) {
        inputHandler->m_mouse.position = { x, y };
        inputHandler->m_mouse.grabbed = true;
      }

      glm::vec2 curr { x, y };
      inputHandler->m_mouse.delta = curr - inputHandler->m_mouse.position;
      inputHandler->m_mouse.position = curr;

      const float sensitivity = 0.1f;
      inputHandler->m_mouse.delta *= sensitivity;
    }

    void InputHandler::MouseCallback(GLFWwindow *handle, int button, int action, int) {
      if (button < 0) return;

      auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
      if (inputHandler == nullptr) return;

      if (action == GLFW_PRESS) {
        inputHandler->m_mouse.buttons.at(button).pressed = true;
        inputHandler->m_mouse.buttons.at(button).down = true;
      }

      if (action == GLFW_RELEASE) {
        inputHandler->m_mouse.buttons.at(button).down = false;
      }

    }

    void InputHandler::ScrollCallback(GLFWwindow *handle, double, double offsetY) {
      auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(handle));
      if (inputHandler == nullptr) return;

      inputHandler->m_mouse.offsetY = static_cast<float>(offsetY);
    }


    auto InputHandler::IsKeyPressed(int key) -> bool {
      bool isPressed = m_keyboard.keys.at(key).pressed;
      m_keyboard.keys.at(key).pressed = false;

      return isPressed;
    }

    auto InputHandler::IsKeyUp(int key) const -> bool {
      return !m_keyboard.keys.at(key).down;
    }

    auto InputHandler::IsKeyDown(int key) const -> bool {
      return m_keyboard.keys.at(key).down;
    }

    auto InputHandler::IsMouseButtonPressed(int button) -> bool {
      bool isPressed = m_mouse.buttons.at(button).pressed;
      m_mouse.buttons.at(button).pressed = false;

      return isPressed;
    }

    auto InputHandler::IsMouseButtonUp(int button) const -> bool {
      return !m_mouse.buttons.at(button).down;
    }

    auto InputHandler::IsMouseButtonDown(int button) const -> bool {
      return m_mouse.buttons.at(button).down;
    }

    auto InputHandler::GetMousePosition() const -> glm::vec2 {
      return m_mouse.position;
    }

    auto InputHandler::GetMousePositionX() const -> float {
      return m_mouse.position.x;
    }

    auto InputHandler::GetMousePositionY() const -> float {
      return m_mouse.position.x;
    }

    auto InputHandler::GetMouseDeltaX() -> float {
      float delta = m_mouse.delta.x;
      m_mouse.delta.x = 0.0f;
      return delta;
    }

    auto InputHandler::GetMouseDeltaY() -> float {
    float delta = m_mouse.delta.y;
      m_mouse.delta.y = 0.0f;
      return delta;
    }

    auto InputHandler::GetMouseWheelMove() const -> float {
      return m_mouse.offsetY;
    }

  }

}
