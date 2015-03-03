#version 150

// Uniforms
// ========
uniform sampler2D u_texture;

// Input
// =====
in vec4 fs_color;
in vec2 fs_texCoords;

// Output
// ======
out vec4 out_color;

// Functions
// =========
void main()
{
  out_color = texture(u_texture, fs_texCoords) + (fs_color / 100000.0);
  //out_color = fs_color;
}
