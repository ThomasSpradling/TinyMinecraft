#include "Application/Window.h"
#include "Events/EventHandler.h"
#include "Events/Input/KeyboardEvents.h"
#include "Events/Input/MouseEvents.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"

namespace TinyMinecraft {

  namespace Application {

    Window::Window(int width, int height, const std::string &name)
      : m_width(width)
      , m_height(height)
    {
      PROFILE_FUNCTION(Window)

      const int DEPTH_BUFFER_SIZE = 32;

      if (!glfwInit()) {
        Utils::Logger::Error("GLFW: Error initializing.");
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
        Utils::Logger::Error("GLFW: Error creating window.");
        exit(1);
      }

      glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      glfwSetWindowUserPointer(m_handle, this);

      glfwSetKeyCallback(m_handle, KeyCallback);
      glfwSetCursorPosCallback(m_handle, CursorCallback);
      glfwSetMouseButtonCallback(m_handle, MouseButtonCallback);
      glfwSetScrollCallback(m_handle, ScrollCallback);

      glfwMakeContextCurrent(m_handle);

      if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        Utils::Logger::Error("GLAD: Error initializing.");
        exit(1);
      }

      glfwWindowHint(GLFW_DEPTH_BITS, DEPTH_BUFFER_SIZE);

      glfwSwapInterval(0);

      Event::EventHandler::On<Event::KeyPressedEvent>([this](const Event::KeyPressedEvent &e) {
        if (e.GetKeyCode() == GLFW_KEY_ESCAPE) {
          Close();
        }
      });

      Utils::Logger::Message("Window created and initialized successfully.");
    }

    Window::~Window() {
      Utils::Logger::Message("Destroying window.");
      glfwTerminate();
    }

    void Window::KeyCallback(GLFWwindow *handle, int key, int, int action, int) {
      if (key < 0) return;

      auto *window = static_cast<Window *>(glfwGetWindowUserPointer(handle));
      if (window == nullptr) return;
      
      switch (action) {
        case GLFW_PRESS:
        {
          Event::KeyPressedEvent event(key, false);
          Event::EventHandler::Trigger(event);
          break;
        }
        case GLFW_REPEAT:
        {
          Event::KeyPressedEvent event(key, true);
          Event::EventHandler::Trigger(event);
          break;
        }
        case GLFW_RELEASE:
        {
          Event::KeyReleasedEvent event(key);
          Event::EventHandler::Trigger(event);
          break;
        }
      }

    }

    void Window::CursorCallback(GLFWwindow *handle, double x, double y) {
      auto *window = static_cast<Window *>(glfwGetWindowUserPointer(handle));
      if (window == nullptr) return;

      if (!window->m_mouse.grabbed) {
        window->m_mouse.position = { x, y };
        window->m_mouse.grabbed = true;
      }

      glm::vec2 curr { x, y };
      window->m_mouse.delta = curr - window->m_mouse.position;
      window->m_mouse.position = curr;

      Event::MouseMovedEvent event(
        static_cast<float>(x), static_cast<float>(y),
        static_cast<float>(window->m_mouse.delta.x), static_cast<float>(window->m_mouse.delta.y)
      );
      Event::EventHandler::Trigger(event);
    }

    void Window::MouseButtonCallback(GLFWwindow *handle, int button, int action, int) {
      if (button < 0) return;

      auto *window = static_cast<Window *>(glfwGetWindowUserPointer(handle));
      if (window == nullptr) return;

      switch (action) {
        case GLFW_PRESS:
        {
          Event::MouseButtonPressedEvent event(static_cast<MouseButton>(button));
          Event::EventHandler::Trigger(event);
        }
        case GLFW_RELEASE:
        {
          Event::MouseButtonReleasedEvent event(static_cast<MouseButton>(button));
          Event::EventHandler::Trigger(event);
        }
      }

    }

    void Window::ScrollCallback(GLFWwindow *handle, double, double offsetY) {
      auto *window = static_cast<Window *>(glfwGetWindowUserPointer(handle));
      if (window == nullptr) return;

      Event::MouseScrolledEvent event(static_cast<float>(offsetY));
      Event::EventHandler::Trigger(event);
    }

    void Window::ErrorCallback(int code, const char *description) {
      Utils::Logger::Error("GLFW error {}: {}", code, description);
    }

  }

}
