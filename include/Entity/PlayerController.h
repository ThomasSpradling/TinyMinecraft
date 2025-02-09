#ifndef PLAYER_CONTROLLER_H_
#define PLAYER_CONTROLLER_H_

#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Entity {

    class PlayerController {
    public:
      void Update();

      [[nodiscard]] inline auto GetPosition() -> glm::vec3 { return m_position; }
      [[nodiscard]] inline auto GetYaw() -> float { return m_yaw; }
      [[nodiscard]] inline auto GetPitch() -> float { return m_pitch; }

    private:
      glm::vec3 m_position { 0.0f };

      glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
      glm::vec3 m_front { 0.0f, 0.0f, -1.0f };

      glm::vec3 m_moveDirection { 0.0f, 0.0f, 0.0f };

      float m_pitch = 0.0f, m_yaw = -90.0f;
      float m_speed = 0.05f;

      [[nodiscard]] auto GetRight() const -> glm::vec3;
    };

  }

}

#endif // PLAYER_CONTROLLER_H_
