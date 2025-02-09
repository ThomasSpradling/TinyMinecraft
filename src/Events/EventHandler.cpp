#include "Events/EventHandler.h"
#include "Utils/Logger.h"

namespace TinyMinecraft {

  namespace Event {

    std::unordered_map<EventType, std::vector<EventHandlerFn>> EventHandler::s_eventListeners;

    void EventHandler::Trigger(const Event &event) {
      for (auto &fn : s_eventListeners[event.GetEventType()]) {
        if (fn) {
          fn(event);
        } else {
          Utils::Logger::Error("Error calling function for event {}.", event.ToString());
        }
      }
    }
  }

}
