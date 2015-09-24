#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/rendering/window.h>
#include <krEngine/rendering/shader.h>

TEST_CASE("Vertex Shader", "[shader]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  // Create window and rendering context
  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Load and Compile")
  {
    RefCountedPtr<VertexShader> pVS;
    pVS = VertexShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    REQUIRE_FALSE(pVS != nullptr);
    pVS = VertexShader::loadAndCompile("<shader>Invalid.vs");
    REQUIRE_FALSE(pVS != nullptr);
    pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    REQUIRE(pVS != nullptr);
  }
}

TEST_CASE("Fragment Shader", "[shader]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  // Create window and rendering context
  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Load and Compile")
  {
    RefCountedPtr<FragmentShader> pFS;
    REQUIRE_FALSE(pFS != nullptr);
    pFS = FragmentShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    REQUIRE_FALSE(pFS != nullptr);
    pFS = FragmentShader::loadAndCompile("<shader>Invalid.fs");
    REQUIRE_FALSE(pFS != nullptr);
    pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    REQUIRE(pFS != nullptr);
  }
}

TEST_CASE("Shader Program", "[shader]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  // Create window and rendering context
  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Attach and Link")
  {
    auto pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    REQUIRE(pVS != nullptr);

    auto pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    REQUIRE(pVS != nullptr);

    auto pProgram = ShaderProgram::link(pVS, pFS);
    REQUIRE(pProgram != nullptr);
  }

  SECTION("Attributes")
  {
    auto pVS = VertexShader::loadAndCompile("<shader>Valid.vs");
    auto pFS = FragmentShader::loadAndCompile("<shader>Valid.fs");
    auto pProgram = ShaderProgram::link(pVS, pFS);

    // TODO Implement me.
  }
}
