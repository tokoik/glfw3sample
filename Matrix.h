#pragma once
#include <cmath>
#include <algorithm>
#include <GL/glew.h>

//
// 変換行列
//
class Matrix
{
  // 変換行列の要素
  GLfloat matrix[16];

public:

  // コンストラクタ
  Matrix() {}

  // 配列の内容で初期化するコンストラクタ
  //   a: GLfloat 型の 16 要素の配列
  Matrix(const GLfloat *a)
  {
    std::copy(a, a + 16, matrix);
  }

  // 行列の要素を右辺値として参照する
  const GLfloat &operator[](std::size_t i) const
  {
    return matrix[i];
  }

  // 行列の要素を左辺値として参照する
  GLfloat &operator[](std::size_t i)
  {
    return matrix[i];
  }

  // 変換行列の配列を返す
  const GLfloat *data() const
  {
    return matrix;
  }

  // 乗算
  //   m: Matrix 型の乗数
  Matrix operator*(const Matrix &m) const
  {
    Matrix t;

    for (int i = 0; i < 16; ++i)
    {
      const int j(i & 3), k(i & ~3);

      t[i] =
        matrix[ 0 + j] * m[k + 0] +
        matrix[ 4 + j] * m[k + 1] +
        matrix[ 8 + j] * m[k + 2] +
        matrix[12 + j] * m[k + 3];
    }

    return t;
  }

 // 単位行列を設定する
  void loadIdentity()
  {
    std::fill(matrix, matrix + 16, 0.0f);
    matrix[ 0] = matrix[ 5] = matrix[10] = matrix[15] = 1.0f;
  }

    // 単位行列を作成する
  static Matrix identity()
  {
    Matrix t;
    t.loadIdentity();
    return t;
  }

  // (x, y, z) だけ平行移動する変換行列を作成する
  static Matrix translate(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t;

    t.loadIdentity();
    t[12] = x;
    t[13] = y;
    t[14] = z;

    return t;
  }

  // (x, y, z) 倍に拡大縮小する変換行列を作成する
  static Matrix scale(GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t;

    t.loadIdentity();
    t[ 0] = x;
    t[ 5] = y;
    t[10] = z;

    return t;
  }

  // (x, y, z) を軸に a 回転する変換行列を作成する
  static Matrix rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
  {
    Matrix t;
    const GLfloat d(sqrt(x * x + y * y + z * z));

    if (d > 0.0f)
    {
      const GLfloat l(x / d), m(y / d), n(z / d);
      const GLfloat l2(l * l), m2(m * m), n2(n * n);
      const GLfloat lm(l * m), mn(m * n), nl(n * l);
      const GLfloat c(cos(a)), c1(1.0f - c), s(sin(a));

      t.loadIdentity();
      t[ 0] = (1.0f - l2) * c + l2;
      t[ 1] = lm * c1 + n * s;
      t[ 2] = nl * c1 - m * s;
      t[ 4] = lm * c1 - n * s;
      t[ 5] = (1.0f - m2) * c + m2;
      t[ 6] = mn * c1 + l * s;
      t[ 8] = nl * c1 + m * s;
      t[ 9] = mn * c1 - l * s;
      t[10] = (1.0f - n2) * c + n2;
    }

    return t;
  }
};
