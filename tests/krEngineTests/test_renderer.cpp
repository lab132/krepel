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

    const auto tw = 512;
    const auto th = 512;

    const auto x = 32;
    const auto y = 32;
    const auto w = x + 256;
    const auto h = y + 256;

    auto left   = float(x) / float(tw);
    auto right  = float(w) / float(tw);
    auto top    = float(h) / float(th);
    auto bottom = float(y) / float(th);

    //vertices[0].pos.Set(-1.0f, -1.0f); // Lower Left
    //vertices[1].pos.Set(-1.0f,  1.0f); // Upper Left
    //vertices[2].pos.Set( 1.0f, -1.0f); // Lower Right
    //vertices[3].pos.Set( 1.0f,  1.0f); // Upper Right

    vertices[0].pos.Set(left  * 2 - 1,bottom * 2 - 1); // Lower Left
    vertices[1].pos.Set(left  * 2 - 1,top    * 2 - 1); // Upper Left
    vertices[2].pos.Set(right * 2 - 1,bottom * 2 - 1); // Lower Right
    vertices[3].pos.Set(right * 2 - 1,top    * 2 - 1); // Upper Right

    vertices[0].color = ezColor::GetRed();
    vertices[1].color = ezColor::GetGreen();
    vertices[2].color = ezColor::GetBlue();
    vertices[3].color = ezColor::GetYellow();

    //vertices[0].texCoords.Set(0.0f, 1.0f); // Upper Left
    //vertices[1].texCoords.Set(0.0f, 0.0f); // Lower Left
    //vertices[2].texCoords.Set(1.0f, 1.0f); // Upper Right
    //vertices[3].texCoords.Set(1.0f, 0.0f); // Lower Right

    vertices[0].texCoords.Set(left,  top);
    vertices[1].texCoords.Set(left,  bottom);
    vertices[2].texCoords.Set(right, top);
    vertices[3].texCoords.Set(right, bottom);

    // Vertex Buffer
    // =============
    auto vb = VertexBuffer::create(BufferUsage::StaticDraw, PrimitiveType::Triangles);
    //EZ_TEST_BOOL(setupLayout(vb, prg, "Vertex").Succeeded());
    setupLayout(vb, prg, "Vertex");
    EZ_TEST_BOOL(uploadData(vb, ezMakeArrayPtr(vertices)).Succeeded());

    // Texture
    // =======
    auto tex = Texture::load("<texture>kitten.dds");
    EZ_TEST_BOOL(isValid(tex));

    auto pSampler = Sampler::create();
    use(pSampler, TextureSlot(0));
    KR_ON_SCOPE_EXIT{ unuse(pSampler, TextureSlot(0)); };

    uploadData(shaderUniformOf(prg, "u_texture"), TextureSlot(0));

    glBindTexture(GL_TEXTURE_2D, tex->getId());

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
