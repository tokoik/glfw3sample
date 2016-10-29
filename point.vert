#version 150 core
uniform float aspect;
in vec4 position;
void main()
{
  gl_Position = vec4(1.0 / aspect, 1.0, 1.0, 1.0) * position;
}
