#version 150 core
uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalMatrix;
const int Lcount = 2;
uniform vec4 Lpos[Lcount];
uniform vec3 Lamb[Lcount];
uniform vec3 Ldiff[Lcount];
uniform vec3 Lspec[Lcount];
layout (std140) uniform Material
{
  vec3 Kamb;
  vec3 Kdiff;
  vec3 Kspec;
  float Kshi;
};
in vec4 position;
in vec3 normal;
out vec3 Idiff;
out vec3 Ispec;
void main()
{
  vec4 P = modelview * position;
  vec3 N = normalize(normalMatrix * normal);
  vec3 V = -normalize(P.xyz);
  Idiff = vec3(0.0);
  Ispec = vec3(0.0);
  for (int i = 0; i < Lcount; ++i)
  {
    vec3 L = normalize((Lpos[i] * P.w - P * Lpos[i].w).xyz);
    vec3 Iamb = Kamb * Lamb[i];
    Idiff += max(dot(N, L), 0.0) * Kdiff * Ldiff[i] + Iamb;
    vec3 H = normalize(L + V);
    Ispec += pow(max(dot(N, H), 0.0), Kshi) * Kspec * Lspec[i];
  }
  gl_Position = projection * P;
}
