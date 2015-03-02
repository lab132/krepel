#version 150

// Uniforms
// ========
uniform vec2 u_origin;
uniform float u_rotation; // radians
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
  vec2 pos = u_origin + vs_position;

  vec2 transformedPos;
  transformedPos.x = pos.x * cos(u_rotation) - pos.y * sin(u_rotation);
  transformedPos.y = pos.x * sin(u_rotation) + pos.y * cos(u_rotation);

  fs_texCoords = vs_texCoords;
  gl_Position = u_projection
              * u_view
              * vec4(transformedPos, 0.0, 1.0);
}
