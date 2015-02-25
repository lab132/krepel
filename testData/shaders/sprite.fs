#version 150

// Uniforms
// ========
uniform sampler2D u_texture;
uniform vec4 u_color;

// Input
// =====
in vec2 fs_texCoords;

// Output
// ======
out vec4 out_color;

// Functions
// =========
void main()
{
  out_color = texture(u_texture, fs_texCoords) * u_color;
}
