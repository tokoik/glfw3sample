#version 150 core
uniform mat4 modelview;
uniform mat4 projection;
in vec4 position;
in vec4 color;
out vec4 vertex_color;
void main()
{
  vertex_color = color;
  gl_Position = projection * modelview * position;
}
