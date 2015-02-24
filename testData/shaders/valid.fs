#version 150

// Uniforms
// ========
uniform vec3 u_color;

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
  out_color = vec4(u_color, 1.0) + fs_color;

  // Prevent linker from optimizing out the tex coords.
  out_color += vec4(fs_texCoords, 1.0, 1.0);
}
