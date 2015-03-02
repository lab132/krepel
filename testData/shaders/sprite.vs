#version 150

// Uniforms
// ========
uniform mat4 u_view;
uniform mat4 u_projection;

// Input
// =====
in vec2 vs_position;
in vec2 vs_texCoords;

// Output
// ======
out vec2 fs_texCoords;

// Functions
// =========
void main()
{
  fs_texCoords = vs_texCoords;
  //gl_Position = u_projection * u_view * vec4(-vs_position, 0.0, 1.0);
  gl_Position = u_projection * u_view * vec4(vs_position, 0.0, 1.0);
}
