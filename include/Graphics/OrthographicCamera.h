#ifndef ORTHOGRAPHIC_CAMERA_H_
#define ORTHOGRAPHIC_CAMERA_H_

#include "Utils/mathgl.h"

namespace Graphics {

class OrthographicCamera {
public:
  OrthographicCamera(float left, float right, float bottom, float top);
  auto GetViewProjectionMatrix() const -> const glm::mat4 & { return m_viewProjection; }
  auto GetViewMatrix() const -> const glm::mat4 & { return m_view; }
  auto GetProjectionMatrix() const -> const glm::mat4 & { return m_projection; }

private:
  glm::mat4 m_viewProjection { 1.0f };
  glm::mat4 m_projection { 1.0f };
  glm::mat4 m_view { 1.0f };
};

}

#endif // ORTHOGRAPHIC_CAMERA_H_
