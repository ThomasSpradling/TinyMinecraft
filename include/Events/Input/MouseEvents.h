#ifndef MOUSE_EVENT_H_
#define MOUSE_EVENT_H_

#include "Application/MouseButton.h"
#include "Events/Event.h"
#include <string>

namespace TinyMinecraft {

  namespace Event {

    class MouseButtonEvent : public Event {
    public:
      explicit MouseButtonEvent(Application::MouseButton mouseButton) : m_mouseButton(mouseButton) {}

      [[nodiscard]] inline auto GetMouseButton() const -> int { return m_mouseButton; }

    protected:
      int m_mouseButton;
    };

    class MouseMovedEvent : public Event {
    public:
      explicit MouseMovedEvent(float xPos, float yPos, float xOffset, float yOffset)
        : m_xPos(xPos), m_yPos(yPos), m_xOffset(xOffset), m_yOffset(yOffset) {}
      
      [[nodiscard]] inline auto GetPosX() const -> float { return m_xPos; }
      [[nodiscard]] inline auto GetPosY() const -> float { return m_yPos; }
      [[nodiscard]] inline auto GetOffsetX() const -> float { return m_xOffset; }
      [[nodiscard]] inline auto GetOffsetY() const -> float { return m_yOffset; }

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::MouseMoved; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;

    private:
      float m_xPos, m_yPos;
      float m_xOffset, m_yOffset;
    };

    class MouseScrolledEvent : public Event {
    public:
      explicit MouseScrolledEvent(float yOffset) : m_yOffset(yOffset) {}

      [[nodiscard]] inline auto GetOffsetY() const -> float { return m_yOffset; }

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::MouseScrolled; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;

    private:
      float m_yOffset;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
      explicit MouseButtonPressedEvent(Application::MouseButton button) : MouseButtonEvent(button) {}

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::MouseButtonPressed; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;

    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
      explicit MouseButtonReleasedEvent(Application::MouseButton button) : MouseButtonEvent(button) {}

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::MouseButtonReleased; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;

    };

  }

}

#endif // MOUSE_EVENT_H_
