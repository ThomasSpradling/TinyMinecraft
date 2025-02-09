#ifndef PLAYER_CAMERA_H_
#define PLAYER_CAMERA_H_

#include "Events/Entity/PlayerEvents.h"
#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Scene {

    class PlayerCamera {
    public:
      PlayerCamera();

      virtual void OnPlayerLook(const Event::PlayerLookedEvent &event) = 0;
      virtual void OnPlayerMove(const Event::PlayerMovedEvent &event) = 0;

      [[nodiscard]] inline auto GetViewProjection() -> glm::mat4 { return m_viewProjection; }
    protected:
      glm::mat4 m_viewProjection;
    };

    class FirstPersonPlayerCamera : public PlayerCamera {
    public:
      FirstPersonPlayerCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_fov(fov), m_aspect(aspectRatio), m_near(nearPlane), m_far(farPlane)
      {}

      void OnPlayerLook(const Event::PlayerLookedEvent &event) override;
      void OnPlayerMove(const Event::PlayerMovedEvent &event) override;

      inline void SetFOV(float value) { m_fov = value; }
      [[nodiscard]] inline auto GetFOV() const -> float { return m_fov; }

      inline void SetAspectRatio(float value) { m_aspect = value; }
      [[nodiscard]] inline auto GetAspectRatio() const -> float { return m_aspect; }

      inline void GetNearPlane(float value) { m_near = value; }
      [[nodiscard]] inline auto GetNearPlane() const -> float { return m_near; }

      inline void GetFarPlane(float value) { m_far = value; }
      [[nodiscard]] inline auto GetFarPlane() const -> float { return m_far; }
    private:
      glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
      glm::vec3 m_front { 0.0f, 0.0f, -1.0f };
      glm::vec3 m_right { 1.0f, 0.0f, 0.0f };
      glm::vec3 m_position { 0.0f };

      float m_fov { 45.0f };
      float m_aspect { 1.6f };
      float m_near = 0.1;
      float m_far = 2048.0f;
    };

  }

}

#endif // PLAYER_CAMERA_H_
