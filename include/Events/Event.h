#ifndef EVENT_H_
#define EVENT_H_

#include "Utils/Printable.h"
#include <string>

namespace TinyMinecraft {

  namespace Event {

    enum EventType {
      //// INPUT EVENTS ////

      // keyboard
      KeyPressed, KeyReleased,

      // mouse
      MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved,

      //// ENTITY EVENTS ////

      // player
      PlayerMove, BlockBreak
    };

    class Event : public Utils::Printable {
    public:
      virtual ~Event() = default;

      virtual auto GetEventType() const -> EventType = 0;
      virtual auto ToString() const -> std::string override = 0;
    };

  }

}

#endif // EVENT_H_
