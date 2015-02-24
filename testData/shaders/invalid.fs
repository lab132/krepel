#version 150

uniform vec3 g_Color;

// Intentional error: No such thing as a vec42.
in vec42 Color;

out vec4 outColor;

void main()
{
  outColor = vec4(g_Color, 1.0) + Color;

  // Invert colors.
  outColor = vec4(1.0) - outColor;
}
