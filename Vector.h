#pragma once
#include <array>

// 変換行列
#include "Matrix.h"

//
// ベクトル
//
using Vector = std::array<GLfloat, 4>;

// 行列とベクトルの乗算
//   m: Matrix 型の行列
//   v: Vector 型のベクトル
Vector operator*(const Matrix &m, const Vector &v)
{
  Vector t;

  for (int i = 0; i < 4; ++i)
  {
    t[i] = m[i] * v[0] + m[i + 4] * v[1] + m[i + 8] * v[2] + m[i + 12] * v[3];
  }

  return t;
}
