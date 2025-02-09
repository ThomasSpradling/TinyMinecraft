#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

#include "Events/Event.h"
#include "Utils/Singleton.h"
#include <functional>
#include <type_traits>
#include <unordered_map>

namespace TinyMinecraft {

  namespace Event {

    using EventHandlerFn = std::function<void(const Event &)>;
    
    template <typename E>
    concept ValidEvent = std::is_base_of_v<Event, E>;

    template <typename F, typename E>
    concept ValidEventFn = std::is_invocable_r_v<void, F, const E &>;
    
    class EventHandler : private Utils::Singleton {
    public:

      template <ValidEvent E, ValidEventFn<E> F>
      static void On(F &&callback) {
        EventType event = E::GetStaticType();

        s_eventListeners[event].push_back([callback](const Event &event) {
          if (const auto *derivedEvent = dynamic_cast<const E *>(&event)) {
            callback(*derivedEvent);
          }
        });
      }

      static void Trigger(const Event &event);

    private:

      static std::unordered_map<EventType, std::vector<EventHandlerFn>> s_eventListeners;

    };

  }

}

#endif // EVENT_HANDLER_H_
