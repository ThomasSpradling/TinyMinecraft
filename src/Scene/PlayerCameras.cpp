#include "Scene/PlayerCameras.h"
#include "Events/Entity/PlayerEvents.h"
#include "Events/EventHandler.h"

namespace TinyMinecraft {

  namespace Scene {

    PlayerCamera::PlayerCamera() {
      Event::EventHandler::On<Event::PlayerLookedEvent>([this](auto e) { OnPlayerLook(e); });
      Event::EventHandler::On<Event::PlayerMovedEvent>([this](auto e) { OnPlayerMove(e); });
    }

    //// FIRST PERSON CAMERA ////

    void FirstPersonPlayerCamera::OnPlayerLook(const Event::PlayerLookedEvent &event) {
      const glm::mat4 projection = glm::perspective(
        glm::radians(m_fov),
        m_aspect,
        m_near,
        m_far
      );

      float yaw = event.GetNewYaw();
      float pitch = event.GetNewPitch();
      
      glm::vec3 direction;
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(pitch));
      direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

      m_front = glm::normalize(direction);
      m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
      m_up = glm::normalize(glm::cross(m_right, m_front));

      const glm::mat4 view = glm::lookAt(m_position, m_position + m_front, m_up);
      m_viewProjection = projection * view;
    }

    void FirstPersonPlayerCamera::OnPlayerMove(const Event::PlayerMovedEvent &event) {
      constexpr float playerHeight = 0.0f;

      const glm::mat4 projection = glm::perspective(
        glm::radians(m_fov),
        m_aspect,
        m_near,
        m_far
      );

      m_position = event.GetNewPos() + glm::vec3(0.0f, playerHeight, 0.0f);
      const glm::mat4 view = glm::lookAt(m_position, m_position + m_front, m_up);
      m_viewProjection = projection * view;
    }

  }

}
