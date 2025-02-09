#include "Events/Input/MouseEvents.h"
#include <sstream>

namespace TinyMinecraft {

  namespace Event {

    //// MOUSE MOVE EVENT ////

    auto MouseMovedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "MouseMoveEvent(";
      ss << "Pos: [" << m_xPos << ", " << m_yPos << "], ";
      ss << "Offset: [" << m_xOffset << ", " << m_yOffset << "]";
      ss << ")";
      return ss.str();
    }

    //// MOUSE SCROLL EVENT ////

    auto MouseScrolledEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "MouseScrollEvent(";
      ss << "OffsetY: " << m_yOffset;
      ss << ")";
      return ss.str();
    } 

    //// MOUSE BUTTON PRESS EVENT ////

    auto MouseButtonPressedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "MouseButtonPressEvent(";
      ss << "button: " << static_cast<int>(m_mouseButton);
      ss << ")";
      return ss.str();
    }

    //// MOUSE BUTTON RELEASE EVENT ////

    auto MouseButtonReleasedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "MouseButtonReleaseEvent(";
      ss << "button: " << static_cast<int>(m_mouseButton);
      ss << ")";
      return ss.str();
    }

  }

}
