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

  // Shaders and Shader Program
  // ==========================
  auto vs = VertexShader::loadAndCompile("<shader>texturedQuad.vs");
  auto fs = FragmentShader::loadAndCompile("<shader>texturedQuad.fs");
  auto prg = ShaderProgram::link(vs, fs);
  EZ_TEST_BOOL(isValid(prg));

  // TODO Should be done in a more sophisticated way.
  glUseProgram(prg->m_glHandle);

  // Vertices
  // ========
  Vertex vertices[4];
  vertices[0].pos.Set(-0.5f, 0.5f);
  vertices[1].pos.Set(0.5f, 0.5f);
  vertices[2].pos.Set(0.5f, -0.5f);
  vertices[3].pos.Set(-0.5f, -0.5f);

  vertices[0].color = ezColor::GetRed();
  vertices[1].color = ezColor::GetGreen();
  vertices[2].color = ezColor::GetBlue();
  vertices[3].color = ezColor::GetGreen();

  vertices[0].texCoords.Set(0.0f, 0.0f);
  vertices[1].texCoords.Set(1.0f, 0.0f);
  vertices[2].texCoords.Set(1.0f, 1.0f);
  vertices[3].texCoords.Set(0.0f, 1.0f);

  // Vertex Buffer
  // =============
  auto vb = VertexBuffer::create();
  //EZ_TEST_BOOL(setupLayout(vb, prg, "Vertex").Succeeded());
  setupLayout(vb, prg, "Vertex");
  EZ_TEST_BOOL(uploadData(vb, ezMakeArrayPtr(vertices)).Succeeded());

  // Texture
  // =======
  if (false)
  {
    auto tex = Texture::load("<texture>test_4x4.bmp");
    EZ_TEST_BOOL(isValid(tex));

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
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
    auto u_texture = glGetUniformLocation(prg->m_glHandle, "u_texture");
    glUniform1i(u_texture, 0);// Assign slot 0 to the texture.
  }

  bind(vb, prg);

  auto now = ezTime::Now();
  auto targetTime = now + ezTime::Seconds(4);
  while(now < targetTime)
  {
    auto dt = ezTime::Now() - now;

    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow);

    now += dt;
  }

  ezStartup::ShutdownEngine();
}
