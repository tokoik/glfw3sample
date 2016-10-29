#pragma once

// 図形の描画
#include "Shape.h"

//
// 三角形による描画
//
class SolidShape
  : public Shape
{
public:

  // コンストラクタ
  //   size: 頂点の位置の次元
  //   vertexcount: 頂点の数
  //   vertex: 頂点属性を格納した配列
  SolidShape(GLint size, GLsizei vertexcount, const Object::Vertex *vertex)
    : Shape(size, vertexcount, vertex)
  {
  }

  // 描画の実行
  virtual void execute() const
  {
    // 三角形で描画する
    glDrawArrays(GL_TRIANGLES, 0, vertexcount);
  }
};
