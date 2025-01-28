#ifndef CAMERA_H_
#define CAMERA_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"

namespace Scene {

class Camera {
public:
  Camera() = default;

  void SetPosition(glm::vec3 pos);
  void SetUpVector(glm::vec3 vec);

  void SetFOV(float angle);
  void SetAspectRatio(float aspectRatio);
  void SetNearPlane(GLfloat nearPlane);
  void SetFarPlane(GLfloat farPlane);

  auto GetViewProjection() -> glm::mat4;

  [[nodiscard]] auto GetFront() const -> glm::vec3;
  [[nodiscard]] auto GetRight() const -> glm::vec3;
  [[nodiscard]] auto GetUp() const -> glm::vec3;

  [[nodiscard]] auto GetYaw() const -> float;
  [[nodiscard]] auto GetPitch() const -> float;

  void Move();
  void SetMoveDirection(const glm::vec3 &direction);
  void UpdateViewDirection(float newYaw, float newPitch);

  [[nodiscard]] auto GetPosition() const -> glm::vec3;

private:
  glm::vec3 m_position { 0.0f };

  glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
  glm::vec3 m_front { 0.0f, 0.0f, -1.0f };

  glm::vec3 m_moveDirection { 0.0f, 0.0f, 0.0f };

  float m_pitch = 0.0f, m_yaw = -90.0f;
  float m_speed = 0.05f;

  float m_fov { 45.0f };
  float m_aspect { 1.6f };
  float m_near = 0.1;
  float m_far = 512.0f;
};

}

#endif // CAMERA_H_
