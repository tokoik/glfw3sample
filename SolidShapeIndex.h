#pragma once

// インデックスを使った図形の描画
#include "ShapeIndex.h"

//
// インデックスを使った三角形による描画
//
class SolidShapeIndex
  : public ShapeIndex
{
public:

  // コンストラクタ
  //   size: 頂点の位置の次元
  //   vertexcount: 頂点の数
  //   vertex: 頂点属性を格納した配列
  //   indexcount: 頂点のインデックスの要素数
  //   index: 頂点のインデックスを格納した配列
  SolidShapeIndex(GLint size, GLsizei vertexcount, const Object::Vertex *vertex,
    GLsizei indexcount, const GLuint *index)
    : ShapeIndex(size, vertexcount, vertex, indexcount, index)
  {
  }

  // 描画の実行
  virtual void execute() const
  {
    // 三角形で描画する
    glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, 0);
  }
};
