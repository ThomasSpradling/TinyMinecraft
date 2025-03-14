#ifndef MOUSE_BUTTON_H_
#define MOUSE_BUTTON_H_

#include <cstdint>

namespace TinyMinecraft {

  namespace Application {

    enum MouseButton : uint16_t {
      Button0 = 0,
      Button1 = 1,
      Button2 = 2,
      Button3 = 3,
      Button4 = 4,
      Button5 = 5,
      Button6 = 6,
      Button7 = 7,

      ButtonLeft = Button0,
      ButtonRight = Button1,
      ButtonMiddle = Button2,
      ButtonLast = Button7
    };

  }

}

#endif // MOUSE_BUTTON_H_
