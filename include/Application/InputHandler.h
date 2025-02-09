#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "Application/MouseButton.h"
#include "Graphics/gfx.h"
#include "Utils/Singleton.h"
#include "Utils/mathgl.h"
#include <array>

namespace TinyMinecraft {

  namespace Application {

    struct Button {
      bool pressed = false;
      bool down = false;
    };

    struct Keyboard {
      std::array<Button, GLFW_KEY_LAST> keys;
    };


    struct Mouse {
      std::array<Button, MouseButton::ButtonLast> buttons;
      glm::vec2 delta { 0.0f }, position { 0.0f };
      bool grabbed = false;

      struct Wheel {
        float offsetY = 0.0f;
      } wheel;
    };

    class InputHandler : public Utils::Singleton {
    public:
      static void Initialize();

      [[nodiscard]] inline static auto IsKeyPressed(int key) -> bool;
      [[nodiscard]] inline static auto IsKeyUp(int key) -> bool { return !IsKeyDown(key); };
      [[nodiscard]] inline static auto IsKeyDown(int key) -> bool { return s_keyboard.keys.at(key).down; };

      [[nodiscard]] static auto IsMouseButtonPressed(int button) -> bool;
      [[nodiscard]] inline static auto IsMouseButtonUp(int button) -> bool { return !IsMouseButtonDown(button); }
      [[nodiscard]] inline static auto IsMouseButtonDown(int button) -> bool { return s_mouse.buttons[button].down; }

      [[nodiscard]] inline static auto GetMousePosition() -> glm::vec2 { return s_mouse.position; }
      [[nodiscard]] inline static auto GetMousePositionX() -> float { return s_mouse.position.x; }
      [[nodiscard]] inline static auto GetMousePositionY() -> float { return s_mouse.position.y; };

      [[nodiscard]] auto static GetMouseDeltaX() -> float;
      [[nodiscard]] auto static GetMouseDeltaY() -> float;

      [[nodiscard]] inline auto GetMouseWheelOffsetY() -> float { return s_mouse.wheel.offsetY; }

    private:
      static constexpr float mouseSensitivity = 0.1f;
      static Keyboard s_keyboard;
      static Mouse s_mouse;
    };

  }

}

#endif // INPUT_HANDLER_H_
