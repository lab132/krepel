#version 150

// Uniforms
// ========

// Input
// =====
in vec2 vs_position;
in vec4 vs_color;
in vec2 vs_texCoords;

// Output
// ======
out vec4 fs_color;
out vec2 fs_texCoords;

// Functions
// =========
void main()
{
  fs_color = vs_color;
  fs_texCoords = vs_texCoords;
  gl_Position = vec4(vs_position, 0.0, 1.0);
}
