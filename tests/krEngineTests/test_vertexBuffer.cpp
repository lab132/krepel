#include <krEngineTests/pch.h>

#include <krEngine/rendering/window.h>
#include <krEngine/rendering/vertexBuffer.h>

#include <Foundation/Reflection/ReflectionUtils.h>

namespace
{
  struct TestLayout
  {
    ezVec2 pos;
    ezColor color = ezColor::White;
    ezVec2 texCoords = ezVec2::ZeroVector();
  };
}

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, TestLayout);

EZ_BEGIN_STATIC_REFLECTED_TYPE(TestLayout, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    // We use the names used in the shader here.
    EZ_MEMBER_PROPERTY("vs_position",  pos),
    EZ_MEMBER_PROPERTY("vs_color",     color),
    EZ_MEMBER_PROPERTY("vs_texCoords", texCoords)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_CREATE_SIMPLE_TEST_GROUP(VertexBuffer);

EZ_CREATE_SIMPLE_TEST(VertexBuffer, Basics)
{
  using namespace kr;

  auto pWindow = Window::open();

  KR_TESTS_RAII_ENGINE_STARTUP;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Create")
  {
    auto pVB = VertexBuffer::create(BufferUsage::StaticDraw, PrimitiveType::Triangles);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Layout and Upload")
  {
    auto pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    auto pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    auto pProgram = ShaderProgram::link(pVS, pFS);

    auto h = pProgram->m_glHandle;
    auto pos = glGetAttribLocation(h, "vs_position");
    auto col = glGetAttribLocation(h, "vs_color");
    auto coord = glGetAttribLocation(h, "vs_texCoords");

    auto pVB = VertexBuffer::create(BufferUsage::StaticDraw, PrimitiveType::Triangles);
    EZ_TEST_BOOL(setupLayout(pVB, pProgram, "TestLayout").Succeeded());
    TestLayout data[4];
    data[0].pos.Set(-1,  1);
    data[1].pos.Set( 1,  1);
    data[2].pos.Set( 1, -1);
    data[3].pos.Set(-1, -1);

    uploadData(pVB, ezMakeArrayPtr(data));
    EZ_TEST_BOOL(uploadData(pVB, ezMakeArrayPtr(data)).Succeeded());
  }
}
