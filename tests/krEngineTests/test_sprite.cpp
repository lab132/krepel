#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/transform2D.h>
#include <krEngine/rendering.h>

#include <CoreUtils/Graphics/Camera.h>

TEST_CASE("Workflow", "[sprite]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  auto pWindow = Window::open();
  pWindow->setClearColor(ezColor::CornflowerBlue);

  KR_TESTS_RAII_ENGINE_STARTUP;

  ezCamera cam;
  cam.SetCameraMode(ezCamera::OrthoFixedWidth,                 // Using an orthographic cam.
                    (float)pWindow->getClientAreaSize().width, // Width of the orthographic cam.
                    0.1f,                                      // Near plane.
                    1.0f);                                     // Far plane.
  cam.LookAt(ezVec3(0, 0, 0.5f), // Camera Position.
             ezVec3(0, 0, 0));   // Target Position.

  Sprite s;
  s.setTexture(Texture::load("<texture>kitten.dds"));
  s.setLocalBounds(ezRectFloat(0, 0, 128, 128));
  initialize(s);
  s.getSampler()->setFiltering(TextureFiltering::Nearest);

  // Handle the close-event of the window.
  bool run = true;
  pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
  {
    if (e.type == WindowEventArgs::ClickClose)
      run = false;
  });

  Renderer::addExtractionListener([&s, &cam](Renderer::Extractor& e)
  {
    auto aspectRatio = 16.0f/9.0f;
    extract(e, cam, aspectRatio);

    Transform2D t(Zero);
    t.m_position.Set(-s.getLocalBounds().width / 2.0f);
    t.m_rotation = ezAngle::Degree(30.0f);
    extract(e, s, t);
  });

  ezTime dt;
  while(run)
  {
    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow);
  }
}
