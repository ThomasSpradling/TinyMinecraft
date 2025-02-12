#ifndef PLAYER_EVENTS_H_
#define PLAYER_EVENTS_H_

#include "Events/Event.h"
#include "Utils/mathgl.h"
#include "World/BlockType.h"

namespace TinyMinecraft {

  namespace Entity {
    class Player;
  }

  namespace Event {

    class PlayerMovedEvent : public Event {
    public:
      explicit PlayerMovedEvent(Entity::Player &player, const glm::vec3 &oldPos, const glm::vec3 &newPos)
        : m_player(player)
        , m_oldPos(oldPos)
        , m_newPos(newPos)
      {}

      [[nodiscard]] inline auto GetPlayer() const -> Entity::Player & { return m_player; }
      [[nodiscard]] inline auto GetOldPos() const -> glm::vec3 { return m_oldPos; }
      [[nodiscard]] inline auto GetNewPos() const -> glm::vec3 { return m_newPos; }

      [[nodiscard]] static inline auto GetStaticType() -> EventType { return EventType::PlayerMove; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      [[nodiscard]] auto ToString() const -> std::string override;
      
    private:
      Entity::Player &m_player;
      glm::vec3 m_oldPos, m_newPos;
    };

    // triggers when player changes camera orientation (e.g. moves mouse while not in UI)
    class PlayerLookedEvent : public Event {
    public:
      explicit PlayerLookedEvent(Entity::Player &player, float oldYaw, float oldPitch, float newYaw, float newPitch)
        : m_player(player)
        , m_oldYaw(oldYaw)
        , m_oldPitch(oldPitch)
        , m_newYaw(newYaw)
        , m_newPitch(newPitch)
      {}

      [[nodiscard]] inline auto GetPlayer() const -> Entity::Player & { return m_player; }
      [[nodiscard]] inline auto GetOldYaw() const -> float { return m_oldYaw; }
      [[nodiscard]] inline auto GetOldPitch() const -> float { return m_oldPitch; }
      [[nodiscard]] inline auto GetNewYaw() const -> float { return m_newYaw; }
      [[nodiscard]] inline auto GetNewPitch() const -> float { return m_newPitch; }

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::PlayerMove; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      auto ToString() const -> std::string override;

    private:
      Entity::Player &m_player;
      float m_oldYaw, m_oldPitch;
      float m_newYaw, m_newPitch;
    };

    class BlockBreakEvent : public Event {
    public:
      explicit BlockBreakEvent(Entity::Player &player, BlockType block, glm::vec3 location)
        : m_player(player)
        , m_block(block)
        , m_location(location)
      {}

      [[nodiscard]] inline auto GetPlayer() -> Entity::Player & { return m_player; }
      [[nodiscard]] inline auto GetBlock() -> BlockType { return m_block; }
      [[nodiscard]] inline auto GetLocation() -> glm::vec3 { return m_location; }

      [[nodiscard]] inline static auto GetStaticType() -> EventType { return EventType::BlockBreak; }
      [[nodiscard]] inline auto GetEventType() const -> EventType override { return GetStaticType(); }

      auto ToString() const -> std::string override;

    private:
      Entity::Player &m_player;
      BlockType m_block;
      glm::vec3 m_location;
    };

  }

}

#endif // PLAYER_EVENTS_H_
