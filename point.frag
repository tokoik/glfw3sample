#version 150 core
in vec3 Idiff;
in vec3 Ispec;
out vec4 fragment;
void main()
{
  fragment = vec4(Idiff + Ispec, 1.0);
}
