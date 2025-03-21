#include "Events/Entity/PlayerEvents.h"
#include <sstream>
#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Event {

    //// PLAYER MOVE EVENT ////

    auto PlayerMovedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "PlayerMoveEvent(";
      ss << "OldPos: " << m_oldPos << ", ";
      ss << "NewPos: " << m_newPos;
      ss << ")";
      return ss.str();
    }

    //// PLAYER LOOK EVENT ////

    auto PlayerLookedEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "PlayerLookEvent(";
      ss << "Old: [Y: " << m_oldYaw << ", P: " << m_oldPitch << "], ";
      ss << "New: [Y: " << m_newYaw << ", P: " << m_newPitch << "]";
      ss << ")";
      return ss.str();
    }

    //// BLOCK BREAK EVENT ////

    auto BlockBreakEvent::ToString() const -> std::string {
      std::stringstream ss;
      ss << "BlockBreakEvent(";
      ss << "Block: " << m_block << ", ";
      ss << "Location: " << m_location;
      ss << ")";
      return ss.str();
    }

  }

}
