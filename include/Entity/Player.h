#ifndef PLAYER_H_
#define PLAYER_H_

#include "Entity/PlayerController.h"
#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Entity {

    class Player {
    public:
      Player(const std::shared_ptr<PlayerController> &controller);

      void Update();

      [[nodiscard]] inline auto GetPosition() -> glm::vec3 { return m_position; }
      [[nodiscard]] inline auto GetYaw() -> float { return m_yaw; }
      [[nodiscard]] inline auto GetPitch() -> float { return m_pitch; }
    private:
      std::shared_ptr<PlayerController> m_controller;

      glm::vec3 m_position;
      float m_yaw, m_pitch;
    };

  }

}

#endif // PLAYER_H_
