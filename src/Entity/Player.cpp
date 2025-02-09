#include "Entity/Player.h"
#include "Events/Entity/PlayerEvents.h"
#include "Events/EventHandler.h"
#include "Utils/Logger.h"

namespace TinyMinecraft {

  namespace Entity {

    Player::Player(const std::shared_ptr<PlayerController> &controller)
      : m_controller(controller)
    {
      m_position = m_controller->GetPosition();
      m_yaw = m_controller->GetYaw();
      m_pitch = m_controller->GetPitch();

      Event::PlayerLookedEvent lookEvent(m_yaw, m_pitch, m_yaw, m_pitch);
      Event::EventHandler::Trigger(lookEvent);

      Event::PlayerMovedEvent moveEvent(m_position, m_position);
      Event::EventHandler::Trigger(moveEvent);
    }
  
    void Player::Update() {
      glm::vec3 oldPosition = m_position;
      float oldYaw = m_yaw;
      float oldPitch = m_pitch;

      m_controller->Update();

      m_position = m_controller->GetPosition();
      m_yaw = m_controller->GetYaw();
      m_pitch = m_controller->GetPitch();
      
      if (oldPosition != m_position) {            
        Event::PlayerMovedEvent event(oldPosition, m_position);
        Event::EventHandler::Trigger(event);
      }

      if (oldYaw != m_yaw || oldPitch != m_pitch) {
        Event::PlayerLookedEvent event(oldYaw, oldPitch, m_yaw, m_pitch);
        Event::EventHandler::Trigger(event);
      }
    }

  }

}