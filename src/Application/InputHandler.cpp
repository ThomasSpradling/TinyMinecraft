#include "Application/InputHandler.h"

namespace Application {

bool InputHandler::IsKeyPressed(int key) {
  bool isPressed = keyboard[key].pressed;
  keyboard[key].pressed = false;

  return isPressed;
}

bool InputHandler::IsKeyDown(int key) {
  return keyboard[key].isDown;
}

bool InputHandler::IsKeyUp(int key) {
  return !keyboard[key].isDown;
}

}
