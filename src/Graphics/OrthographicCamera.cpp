#include "Graphics/OrthographicCamera.h"

namespace TinyMinecraft {

  namespace Graphics {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top){
      m_projection = glm::ortho(left, right, bottom, top);
      m_viewProjection = m_projection * m_view;
    }

  }

}
