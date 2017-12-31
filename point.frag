#version 150 core
const int Lcount = 2;
uniform vec4 Lpos[Lcount];
uniform vec3 Lspec[Lcount];
layout (std140) uniform Material
{
  vec3 Kamb;
  vec3 Kdiff;
  vec3 Kspec;
  float Kshi;
};
in vec3 Idiff;
in vec4 P;
in vec3 N;
out vec4 fragment;
void main()
{
  vec3 V = -normalize(P.xyz);
  vec3 Ispec = vec3(0.0);
  for (int i = 0; i < Lcount; ++i)
  {
    vec3 L = normalize((Lpos[i] * P.w - P * Lpos[i].w).xyz);
    vec3 H = normalize(L + V);
    Ispec += pow(max(dot(normalize(N), H), 0.0), Kshi) * Kspec * Lspec[i];
  }
  fragment = vec4(Idiff + Ispec, 1.0);
}
