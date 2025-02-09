#include "Application/InputHandler.h"
#include "Events/EventHandler.h"
#include "Events/Input/KeyboardEvents.h"
#include "Events/Input/MouseEvents.h"
#include "Utils/Logger.h"
#include "glm/fwd.hpp"

namespace TinyMinecraft {

  namespace Application {

    Keyboard InputHandler::s_keyboard;
    Mouse InputHandler::s_mouse;

    void InputHandler::Initialize() {
      Event::EventHandler::On<Event::KeyPressedEvent>([](const Event::KeyPressedEvent &e) {
        s_keyboard.keys[e.GetKeyCode()].pressed = true;
        s_keyboard.keys[e.GetKeyCode()].down = true;
      });

      Event::EventHandler::On<Event::KeyReleasedEvent>([](const Event::KeyReleasedEvent &e) {
        s_keyboard.keys[e.GetKeyCode()].down = false;
      });

      Event::EventHandler::On<Event::MouseMovedEvent>([](const Event::MouseMovedEvent &e) {
        s_mouse.delta.x = e.GetOffsetX();
        s_mouse.delta.y = e.GetOffsetY();

        s_mouse.position.x = e.GetPosX();
        s_mouse.position.y = e.GetPosY();

        s_mouse.delta *= mouseSensitivity;
      });

      Event::EventHandler::On<Event::MouseButtonPressedEvent>([](const Event::MouseButtonPressedEvent &e) {
        s_mouse.buttons[e.GetMouseButton()].pressed = true;
        s_mouse.buttons[e.GetMouseButton()].down = true;
      });

      Event::EventHandler::On<Event::MouseButtonReleasedEvent>([](const Event::MouseButtonReleasedEvent &e) {
        s_mouse.buttons[e.GetMouseButton()].down = false;
      });

      Event::EventHandler::On<Event::MouseScrolledEvent>([](const Event::MouseScrolledEvent &e) {
        s_mouse.wheel.offsetY = e.GetOffsetY();
      });
    }

    auto InputHandler::IsKeyPressed(int key) -> bool {
      bool isPressed = s_keyboard.keys.at(key).pressed;
      s_keyboard.keys.at(key).pressed = false;

      return isPressed;
    }

    auto InputHandler::GetMouseDeltaX() -> float {
      float delta = s_mouse.delta.x;
      s_mouse.delta.x = 0.0f;
      return delta;
    }

    auto InputHandler::GetMouseDeltaY() -> float {
      float delta = s_mouse.delta.y;
      s_mouse.delta.y = 0.0f;
      return delta;
    }

    auto InputHandler::IsMouseButtonPressed(int button) -> bool {
      bool isPressed = s_mouse.buttons.at(button).pressed;
      s_mouse.buttons.at(button).pressed = false;

      return isPressed;
    }

  }

}
