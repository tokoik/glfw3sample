#version 150 core
uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalMatrix;
in vec4 position;
in vec3 normal;
out vec4 P;
out vec3 N;
void main()
{
  P = modelview * position;
  N = normalize(normalMatrix * normal);
  gl_Position = projection * P;
}
