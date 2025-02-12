#ifndef GAME_H_
#define GAME_H_

#include "Entity/Player.h"
#include "Graphics/OrthographicCamera.h"
#include "Graphics/Renderer.h"
#include "Application/Window.h"
#include "Scene/PlayerCameras.h"
#include "World/World.h"

namespace TinyMinecraft {

  namespace Application {

    class Game {
    public:
      Game();

      /* Runs the Update() and Render() functions via a fixed-timestep game loop. */
      void Run();

    private:
      static constexpr int viewportWidth = 1920;
      static constexpr int viewportHeight = 1080;

      Window m_window;
      Graphics::Renderer m_renderer;
      std::shared_ptr<World::World> m_world;
      UI::UserInterface m_ui;
      
      std::shared_ptr<Scene::PlayerCamera> m_camera;
      Graphics::OrthographicCamera m_ortho;

      Entity::Player m_player;

      /* Game updating logic. */
      void Update();

      /* Game rendering logic where `0 <= interpolation <= 1` is the fraction of a frame we intend to try
        to render ahead of the current frame. */
      void Render(double interpolation);
    };

  }

}

#endif // GAME_H_