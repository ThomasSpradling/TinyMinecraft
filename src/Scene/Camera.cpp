#include "Scene/Camera.h"
#include "Utils/defs.h"
#include "glm/geometric.hpp"

namespace TinyMinecraft {

  namespace Scene {

    void Camera::SetPosition(glm::vec3 pos) { m_position = pos; }
    void Camera::SetUpVector(glm::vec3 vec) { m_up = vec; }

    void Camera::SetFOV(float angle) { m_fov = angle; }
    void Camera::SetAspectRatio(float aspectRatio) { m_aspect = aspectRatio; }
    void Camera::SetNearPlane(GLfloat nearPlane) { m_near = nearPlane; }
    void Camera::SetFarPlane(GLfloat farPlane) { m_far = farPlane; }

    auto Camera::GetViewProjection() -> glm::mat4 {
      glm::mat4 projection = glm::perspective(
        m_fov,
        m_aspect,
        m_near,
        m_far
      );

      glm::mat4 view = glm::lookAt(m_position, m_position + m_front, GetUp());

      return projection * view;
    }

    auto Camera::GetFront() const -> glm::vec3 {
      return m_front;
    }

    auto Camera::GetRight() const -> glm::vec3 {
      return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    auto Camera::GetUp() const -> glm::vec3 {
      return glm::normalize(glm::cross(GetRight(), GetFront()));
    }

    auto Camera::GetYaw() const -> float {
      return m_yaw;
    }

    auto Camera::GetPitch() const -> float {
      return m_pitch;
    }

    void Camera::Move() {
      m_position += m_moveDirection != glm::vec3(0.0f) ? (m_speed * FIXED_UPDATE_INTERVAL * glm::normalize(m_moveDirection)) : m_moveDirection;
    }

    void Camera::SetMoveDirection(const glm::vec3 &direction) {
      m_moveDirection = direction;
    }

    void Camera::UpdateViewDirection(float newYaw, float newPitch) {
      m_yaw = newYaw;
      m_pitch = newPitch;

      glm::vec3 direction;
      direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
      direction.y = sin(glm::radians(m_pitch));
      direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

      m_front = glm::normalize(direction);
    }

    auto Camera::GetPosition() const -> glm::vec3 {
      return m_position;
    }

  }

}
