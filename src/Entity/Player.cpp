#include "Entity/Player.h"
#include "Events/Entity/PlayerEvents.h"
#include "Events/EventHandler.h"
#include "Geometry/geometry.h"
#include "Graphics/WireframeRenderer.h"
#include "Utils/Logger.h"
#include "World/BlockType.h"

namespace TinyMinecraft {

  namespace Entity {

    Player::Player(const std::shared_ptr<PlayerController> &controller)
      : m_controller(controller)
    {
      m_position = m_controller->GetPosition();
      m_yaw = m_controller->GetYaw();
      m_pitch = m_controller->GetPitch();

      Event::PlayerLookedEvent lookEvent(*this, m_yaw, m_pitch, m_yaw, m_pitch);
      Event::EventHandler::Trigger(lookEvent);

      Event::PlayerMovedEvent moveEvent(*this, m_position, m_position);
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
        Event::PlayerMovedEvent event(*this, oldPosition, m_position);
        Event::EventHandler::Trigger(event);
      }

      if (oldYaw != m_yaw || oldPitch != m_pitch) {
        Event::PlayerLookedEvent event(*this, oldYaw, oldPitch, m_yaw, m_pitch);
        Event::EventHandler::Trigger(event);
      }
      
      // update block looking at
      constexpr float playerHeight = 0.0f;
      glm::vec3 lookPosition = m_position + glm::vec3(0.0f, playerHeight, 0.0f);

      m_targetingBlock = m_world->ComputeBlockRayInteresection(Geometry::Ray{ lookPosition, m_controller->GetFront(), blockBreakDistance });

      if (m_controller->IsActionPressed()) {
        HandleActionButton();
        m_controller->SetActionPressed(false);
      }
    }

    void Player::BreakBlock() {
      auto [location, face, block] = m_targetingBlock;
      m_world->BreakBlock(location);
    }

    void Player::HandleActionButton() {
      auto [location, face, block] = m_targetingBlock;
      if (block != BlockType::Air && face != Geometry::Face::None) {
        BreakBlock();
      }
    }

  }

}