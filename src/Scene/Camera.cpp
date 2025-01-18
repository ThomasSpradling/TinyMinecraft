#include "Scene/Camera.h"
#include "glm/geometric.hpp"

namespace Scene {

void Camera::SetPosition(glm::vec3 pos) { position = pos; }
// void Camera::SetTarget(glm::vec3 pos) { target = pos; }
void Camera::SetUpVector(glm::vec3 vec) { up = vec; }

void Camera::SetFOV(float angle) { fov = angle; }
void Camera::SetAspectRatio(float aspectRatio) { aspect = aspectRatio; }
void Camera::SetNearPlane(GLfloat nearPlane) { near = nearPlane; }
void Camera::SetFarPlane(GLfloat farPlane) { far = farPlane; }

glm::mat4 Camera::GetViewProjection() {
  glm::mat4 projection = glm::perspective(
    fov,
    aspect,
    near,
    far
  );

  glm::mat4 view = glm::lookAt(position, position + front, GetUp());

  return projection * view;
}

glm::vec3 Camera::GetFront() const {
  return front;
}

glm::vec3 Camera::GetRight() const {
  return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Camera::GetUp() const {
  return glm::normalize(glm::cross(GetRight(), GetFront()));
}

float Camera::GetYaw() const {
  return yaw;
}

float Camera::GetPitch() const {
  return pitch;
}

void Camera::Move(const glm::vec3 &direction, float dt) {
  float displacement = speed * dt;
  position += direction != glm::vec3(0.0f) ? displacement * glm::normalize(direction) : direction;
}

void Camera::UpdateViewDirection(float newYaw, float newPitch) {
  yaw = newYaw;
  pitch = newPitch;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  front = glm::normalize(direction);
}

}
