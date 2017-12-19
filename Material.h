#pragma once
#include <array>
#include <GL/glew.h>

//
// 材質データ
//
struct Material
{
  // 環境光の反射係数
  alignas(16) std::array<GLfloat, 3> ambient;

  // 拡散反射係数
  alignas(16) std::array<GLfloat, 3> diffuse;

   // 鏡面反射係数
  alignas(16) std::array<GLfloat, 3> specular;

  // 輝き係数
  alignas(4) GLfloat shininess;
};
