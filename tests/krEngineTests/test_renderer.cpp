#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/window.h>
#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/texture.h>

namespace
{
  struct Vertex
  {
    ezVec2 pos;
    ezColor color = ezColor::GetWhite();
    ezVec2 texCoords = ezVec2::ZeroVector();
  };
}

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, Vertex);

EZ_BEGIN_STATIC_REFLECTED_TYPE(Vertex, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    // We use the names used in the shader here.
    EZ_MEMBER_PROPERTY("vs_position",  pos),
    EZ_MEMBER_PROPERTY("vs_color",     color),
    EZ_MEMBER_PROPERTY("vs_texCoords", texCoords)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_CREATE_SIMPLE_TEST_GROUP(Renderer);

EZ_CREATE_SIMPLE_TEST(Renderer, Experiments)
{
  using namespace kr;

  ezWindowCreationDesc desc;
  desc.m_ClientAreaSize.width = 600;
  desc.m_ClientAreaSize.height = 480;
  desc.m_Title = "Sprite Test";
  auto pWindow = Window::open(desc);
  EZ_TEST_BOOL(isValid(pWindow));
  pWindow->setClearColor(ezColor::GetCornflowerBlue());

  ezStartup::StartupEngine();

  {
    // Shaders and Shader Program
    // ==========================
    auto vs = VertexShader::loadAndCompile("<shader>texturedQuad.vs");
    auto fs = FragmentShader::loadAndCompile("<shader>texturedQuad.fs");
    auto prg = ShaderProgram::link(vs, fs);
    EZ_TEST_BOOL(isValid(prg));

    use(prg);
    KR_ON_SCOPE_EXIT{ unuse(prg); };

    // Vertices
    // ========
    Vertex vertices[4];
    vertices[0].pos.Set(-0.5f, -0.5f); // Lower Left
    vertices[1].pos.Set(-0.5f,  0.5f); // Upper Left
    vertices[2].pos.Set( 0.5f, -0.5f); // Lower Right
    vertices[3].pos.Set( 0.5f,  0.5f); // Upper Right

    vertices[0].color = ezColor::GetRed();
    vertices[1].color = ezColor::GetGreen();
    vertices[2].color = ezColor::GetBlue();
    vertices[3].color = ezColor::GetYellow();

    vertices[0].texCoords.Set(0.0f, 1.0f); // Upper Left
    vertices[1].texCoords.Set(0.0f, 0.0f); // Lower Left
    vertices[2].texCoords.Set(1.0f, 1.0f); // Upper Right
    vertices[3].texCoords.Set(1.0f, 0.0f); // Lower Right

    // Vertex Buffer
    // =============
    auto vb = VertexBuffer::create(BufferUsage::StaticDraw, PrimitiveType::Triangles);
    //EZ_TEST_BOOL(setupLayout(vb, prg, "Vertex").Succeeded());
    setupLayout(vb, prg, "Vertex");
    EZ_TEST_BOOL(uploadData(vb, ezMakeArrayPtr(vertices)).Succeeded());

    // Texture
    // =======
    auto tex = Texture::load("<texture>kitten.dds");
    if(true)
    {
      EZ_TEST_BOOL(isValid(tex));

      glActiveTexture(tex->getUnit());
      glBindTexture(GL_TEXTURE_2D, tex->getId());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      auto pixels = tex->getImage().GetSubImagePointer<void>();
      glTexImage2D(GL_TEXTURE_2D,    // Target
                   0,                // (Mip) Level
                   GL_RGBA,          // Format used by OpenGL, not the pixels.
                   tex->getWidth(),  // Texture width
                   tex->getHeight(), // Texture height
                   0,                // Must be 0, as per the specification.
                   GL_BGRA,          // Format of the pixels (see last argument).
                   GL_UNSIGNED_BYTE, // Size per pixel.
                   pixels);          // The actual pixel data.
      // TODO This must be done in a more sophisticated way.
      uploadUniformValue(shaderUniformOf(prg, "u_texture"), tex);
    }

    EZ_TEST_BOOL(use(vb, prg).Succeeded());
    KR_ON_SCOPE_EXIT{ unuse(vb, prg); };

    bool run = true;

    pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
    {
      if (e.type == WindowEventArgs::ClickClose)
        run = false;
    });

    auto now = ezTime::Now();
    //auto targetTime = now + ezTime::Seconds(2);
    //while(now < targetTime)
    while(run)
    {
      auto dt = ezTime::Now() - now;

      processWindowMessages(pWindow);
      Renderer::extract();
      Renderer::update(dt, pWindow);

      now += dt;
    }
  }

  ezStartup::ShutdownEngine();
}
