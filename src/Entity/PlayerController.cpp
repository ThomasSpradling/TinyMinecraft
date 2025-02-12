#include "Entity/PlayerController.h"
#include "Application/InputHandler.h"
#include "Application/MouseButton.h"
#include "Events/Entity/PlayerEvents.h"
#include "Events/Event.h"
#include "Events/EventHandler.h"
#include "Events/Input/MouseEvents.h"
#include "Utils/defs.h"
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TinyMinecraft {
  namespace Entity {

    void PlayerController::Update() {
      constexpr float maxPitch = 89.f;
      constexpr float epsilon = 0.0001f;

      glm::vec3 rightDirection = GetRight();

      glm::vec3 direction { 0.f };
      if (Application::InputHandler::IsKeyDown(GLFW_KEY_W)) {
        direction += m_front;
      }
      if (Application::InputHandler::IsKeyDown(GLFW_KEY_S))
        direction -= m_front;
      if (Application::InputHandler::IsKeyDown(GLFW_KEY_D))
        direction += rightDirection;
      if (Application::InputHandler::IsKeyDown(GLFW_KEY_A))
        direction -= rightDirection;

      if (Application::InputHandler::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
        direction -= glm::vec3(0.0f, 1.0f, 0.0f);
      if (Application::InputHandler::IsKeyDown(GLFW_KEY_SPACE))
        direction += glm::vec3(0.0f, 1.0f, 0.0f);
      
      if (glm::length(direction) > epsilon) {
        direction = glm::normalize(direction);
      }

      m_yaw += Application::InputHandler::GetMouseDeltaX();
      m_pitch -= Application::InputHandler::GetMouseDeltaY();
      m_pitch = std::clamp(m_pitch, -maxPitch, maxPitch);

      if(glm::length(direction) > epsilon)
        m_position += m_speed * FIXED_UPDATE_INTERVAL * direction;
      else
        m_position += m_moveDirection;

      m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
      m_front.y = sin(glm::radians(m_pitch));
      m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
      m_front = glm::normalize(m_front);
    
      if (Application::InputHandler::IsMouseButtonPressed(Application::MouseButton::ButtonLeft)) {
        m_isActionPressed = true;
      }
    }

    auto PlayerController::GetRight() const -> glm::vec3 {
      return glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
  }
}
