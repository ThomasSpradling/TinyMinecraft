#ifndef CAMERA_H_
#define CAMERA_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"

namespace Scene {

class Camera {
public:
  Camera() = default;
  ~Camera() = default;

  void SetPosition(glm::vec3 pos);
  void SetUpVector(glm::vec3 vec);

  void SetFOV(float angle);
  void SetAspectRatio(float aspectRatio);
  void SetNearPlane(GLfloat nearPlane);
  void SetFarPlane(GLfloat farPlane);

  glm::mat4 GetViewProjection();

  glm::vec3 GetFront() const;
  glm::vec3 GetRight() const;
  glm::vec3 GetUp() const;

  float GetYaw() const;
  float GetPitch() const;

  void Move();
  void SetMoveDirection(const glm::vec3 &direction);
  void UpdateViewDirection(float newYaw, float newPitch);

  glm::vec3 &GetPosition();

private:
  glm::vec3 position { 0.0f };

  glm::vec3 up { 0.0f, 1.0f, 0.0f };
  glm::vec3 front { 0.0f, 0.0f, -1.0f };

  glm::vec3 moveDirection { 0.0f, 0.0f, 0.0f };

  float pitch = 0.0f, yaw = -90.0f;

  const float speed = 0.02f;

  float fov { 45.0f };
  float aspect { 1.6f };
  float near = 0.1;
  float far = 256.0f;
};

}

#endif // CAMERA_H_
