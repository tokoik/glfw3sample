#pragma once
#include <memory>
#include <GL/glew.h>

//
// ユニフォームバッファオブジェクト
//
template <typename T>
class Uniform
{
  struct UniformBuffer
  {
    // ユニフォームバッファオブジェクト名
    GLuint ubo;

    // コンストラクタ
    //   data: uniform ブロックに格納するデータ
    UniformBuffer(const T *data)
    {
      // ユニフォームバッファオブジェクトを作成する
      glGenBuffers(1, &ubo);
      glBindBuffer(GL_UNIFORM_BUFFER, ubo);
      glBufferData(GL_UNIFORM_BUFFER,
        sizeof (T), data, GL_STATIC_DRAW);
    }

    // デストラクタ
    ~UniformBuffer()
    {
      // ユニフォームバッファオブジェクトを削除する
      glDeleteBuffers(1, &ubo);
    }
  };

  // バッファオブジェクト
  const std::shared_ptr<const UniformBuffer> buffer;

public:

  // コンストラクタ
  //   data: uniform ブロックに格納するデータ
  Uniform(const T *data = NULL)
    : buffer(new UniformBuffer(data))
  {
  }

  // デストラクタ
  virtual ~Uniform()
  {
  }

  // ユニフォームバッファオブジェクトにデータを格納する
  //   data: uniform ブロックに格納するデータ
  void set(const T *data) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
      sizeof (T), data);
  }

  // このユニフォームバッファオブジェクトを使用する
  //   bp: 結合ポイント
  void select(GLuint bp) const
  {
    // 結合ポイントにユニフォームバッファオブジェクトを結合する
    glBindBufferBase(GL_UNIFORM_BUFFER, bp,
      buffer->ubo);
  }
};
