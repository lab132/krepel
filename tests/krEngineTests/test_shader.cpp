#include <krEngine/rendering/window.h>
#include <krEngine/rendering/shader.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Shader);

EZ_CREATE_SIMPLE_TEST(Shader, VertexShader)
{
  using namespace kr;

  // Create window and rendering context
  auto pWindow = Window::open();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Load and Compile")
  {
    RefCountedPtr<VertexShader> pVS;
    pVS = VertexShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    EZ_TEST_BOOL(!isValid(pVS));
    pVS = VertexShader::loadAndCompile("<shader>Invalid.vs");
    EZ_TEST_BOOL(!isValid(pVS));
    pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    EZ_TEST_BOOL(isValid(pVS));
  }
}

EZ_CREATE_SIMPLE_TEST(Shader, FragmentShader)
{
  using namespace kr;

  // Create window and rendering context
  auto pWindow = Window::open();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Load and Compile")
  {
    RefCountedPtr<FragmentShader> pFS;
    EZ_TEST_BOOL(!isValid(pFS));
    pFS = FragmentShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    EZ_TEST_BOOL(!isValid(pFS));
    pFS = FragmentShader::loadAndCompile("<shader>Invalid.fs");
    EZ_TEST_BOOL(!isValid(pFS));
    pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    EZ_TEST_BOOL(isValid(pFS));
  }
}

EZ_CREATE_SIMPLE_TEST(Shader, ShaderProgram)
{
  using namespace kr;

  // Create window and rendering context
  auto pWindow = Window::open();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Attach and Link")
  {
    auto pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    EZ_TEST_BOOL(isValid(pVS));

    auto pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    EZ_TEST_BOOL(isValid(pVS));

    auto pProgram = ShaderProgram::link(pVS, pFS);
    EZ_TEST_BOOL(isValid(pProgram));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Attributes")
  {
    auto pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    auto pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    auto pProgram = ShaderProgram::link(pVS, pFS);

    // TODO Implement me.
  }
}
