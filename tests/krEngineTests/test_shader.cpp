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
    Owned<VertexShader> vs;
    vs = VertexShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    REQUIRE(vs == nullptr);
    vs = VertexShader::loadAndCompile("<shader>Invalid.vs");
    REQUIRE(vs == nullptr);
    vs = VertexShader::loadAndCompile("<shader>Valid.vs");
    REQUIRE(vs != nullptr);
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
    Owned<FragmentShader> fs;
    fs = FragmentShader::loadAndCompile("<What The Hell>I don't exist.nopes");
    REQUIRE(fs == nullptr);
    fs = FragmentShader::loadAndCompile("<shader>Invalid.fs");
    REQUIRE(fs == nullptr);
    fs = FragmentShader::loadAndCompile("<shader>Valid.fs");
    REQUIRE(fs != nullptr);
  }
}

TEST_CASE("Shader Program", "[shader]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  // Create window and rendering context
  auto window = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Link")
  {
    auto vs = VertexShader::loadAndCompile("<shader>Valid.vs");
    REQUIRE(vs != nullptr);

    auto fs = FragmentShader::loadAndCompile("<shader>Valid.fs");
    REQUIRE(fs != nullptr);

    auto shader = ShaderProgram::link(borrow(vs), borrow(fs));
    REQUIRE(shader != nullptr);
  }

  SECTION("Load and Link")
  {
    auto shader = ShaderProgram::loadAndLink("<shader>Valid.vs", "<shader>Valid.fs");
    REQUIRE(shader != nullptr);
  }

  SECTION("Attributes")
  {
    auto vs = VertexShader::loadAndCompile("<shader>Valid.vs");
    auto fs = FragmentShader::loadAndCompile("<shader>Valid.fs");
    auto shader = ShaderProgram::link(borrow(vs), borrow(fs));

    // TODO Implement me.
  }
}
