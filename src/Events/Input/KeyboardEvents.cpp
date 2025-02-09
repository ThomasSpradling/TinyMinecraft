#include "Events/Input/KeyboardEvents.h"
#include <sstream>

namespace TinyMinecraft {

  namespace Event {

    //// KEY PRESS EVENT ////

    auto KeyPressedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "KeyPressEvent(";
      ss << "key: " << m_keyCode;
      ss << ")";
      return ss.str();
    }

    //// KEY RELEASE EVENT ////

    auto KeyReleasedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "KeyReleaseEvent(";
      ss << "key: " << m_keyCode;
      ss << ")";
      return ss.str();
    }

  }

}
