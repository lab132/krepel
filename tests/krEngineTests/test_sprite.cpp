#include <krEngine/transform2D.h>
#include <krEngine/rendering.h>

#include <CoreUtils/Graphics/Camera.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Workflow)
{
  using namespace kr;

  auto pWindow = Window::open();
  pWindow->setClearColor(ezColor::GetCornflowerBlue());

  ezCamera cam;
  cam.SetCameraMode(ezCamera::OrthoFixedWidth,                 // Using an orthographic cam.
                    (float)pWindow->getClientAreaSize().width, // Width of the orthographic cam.
                    0.1f,                                      // Near plane.
                    1.0f);                                     // Far plane.
  cam.LookAt(ezVec3(0, 0, 1),  // Camera Position.
             ezVec3(0, 0, 0)); // Target Position.

  ezMat4 view, projection;
  cam.GetViewMatrix(view);
  cam.GetProjectionMatrix(16.0f/9.0f, ezProjectionDepthRange::MinusOneToOne, projection);

  Sprite s;
  s.setTexture(Texture::load("<texture>kitten.dds"));
  initialize(s);
  s.getSampler()->setFiltering(TextureFiltering::Nearest);

  ezStartup::StartupEngine();

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
    extract(e, s, t);
  });

  ezTime dt;
  while(run)
  {
    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow);
  }

  ezStartup::ShutdownEngine();
}
