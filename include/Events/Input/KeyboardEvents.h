#ifndef KEYBOARD_EVENTS_H_
#define KEYBOARD_EVENTS_H_

#include "Events/Event.h"
#include <string>

namespace TinyMinecraft {

  namespace Event {
    
    class KeyEvent : public Event {
    public:
      explicit KeyEvent(int keyCode) : m_keyCode(keyCode) {}
      
      [[nodiscard]] inline auto GetKeyCode() const -> int { return m_keyCode; }

    protected:
      int m_keyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
      explicit KeyPressedEvent(int keyCode, bool isRepeating)
        : KeyEvent(keyCode)
        , m_isRepeating(isRepeating)
      {}

      [[nodiscard]] inline auto IsRepeating() const -> bool { return m_isRepeating; }

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::KeyPressed; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;

    private:
      bool m_isRepeating;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
      explicit KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::KeyPressed; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;
    };

  }

}

#endif // KEYBOARD_EVENTS_H_
