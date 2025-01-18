#include "Scene/Camera.h"

namespace Scene {

void Camera::SetPosition(glm::vec3 pos) { position = pos; }
void Camera::SetTarget(glm::vec3 pos) { target = pos; }
void Camera::SetUpVector(glm::vec3 vec) { up = vec; }

void Camera::SetFOV(float angle) { fov = angle; }
void Camera::SetAspectRatio(float aspectRatio) { aspect = aspectRatio; }
void Camera::SetNearPlane(GLfloat nearPlane) { near = nearPlane; }
void Camera::SetFarPlane(GLfloat farPlane) { far = farPlane; }

auto Camera::GetViewProjection() -> glm::mat4 {
  glm::mat4 projection = glm::perspective(
    fov,
    aspect,
    near,
    far
  );

  glm::mat4 view = glm::lookAt(position, target, up);

  return projection * view;
}

}
