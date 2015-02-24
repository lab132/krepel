#version 150

in vec2 Pos;
in vec42 Col; // There is no such thing a vec42...

out vec4 Color;

void main()
{
  Color = Col;
  gl_Position = vec4(-Pos, 0.0, 1.0);
}
