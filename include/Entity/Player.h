#ifndef PLAYER_H_
#define PLAYER_H_

#include "Entity/PlayerController.h"
#include "Events/Entity/PlayerEvents.h"
#include "Geometry/geometry.h"
#include "Utils/mathgl.h"
#include "World/World.h"

namespace TinyMinecraft {

  namespace Entity {

    class Player {
    public:
      Player(const std::shared_ptr<PlayerController> &controller);

      void Update();
      void BreakBlock();
      
      inline auto GetWorld() -> std::shared_ptr<World::World> { return m_world; }
      inline void SetWorld(std::shared_ptr<World::World> &world) { m_world = world; }
      [[nodiscard]] inline auto GetTargetingBlock() -> World::BlockLocation { return m_targetingBlock; }
      
      [[nodiscard]] inline auto GetPosition() -> glm::vec3 { return m_position; }
      [[nodiscard]] inline auto GetYaw() -> float { return m_yaw; }
      [[nodiscard]] inline auto GetPitch() -> float { return m_pitch; }
      private:
      static constexpr int blockBreakDistance = 10;
      
      std::shared_ptr<PlayerController> m_controller;
      
      std::shared_ptr<World::World> m_world;
      World::BlockLocation m_targetingBlock;
      
      glm::vec3 m_position;
      float m_yaw, m_pitch;

      void HandleActionButton();
    };

  }

}

#endif // PLAYER_H_
