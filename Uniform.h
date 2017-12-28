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

    // ユニフォームブロックのサイズ
    GLsizeiptr blocksize;

    // コンストラクタ
    //   data: uniform ブロックに格納するデータ
    //   count: 確保する uniform ブロックの数
    UniformBuffer(const T *data, unsigned int count)
    {
      // ユニフォームブロックのサイズを求める
      GLint alignment;
      glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
      blocksize = (((sizeof (T) - 1) / alignment) + 1) * alignment;

      // ユニフォームバッファオブジェクトを作成する
      glGenBuffers(1, &ubo);
      glBindBuffer(GL_UNIFORM_BUFFER, ubo);
      glBufferData(GL_UNIFORM_BUFFER,
        count * blocksize, NULL, GL_STATIC_DRAW);
      for (unsigned int i = 0; i < count; ++i)
      {
        glBufferSubData(GL_UNIFORM_BUFFER, i * blocksize,
          sizeof (T), data + i);
      }
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
  //   count: 確保する uniform ブロックの数
  Uniform(const T *data = NULL, unsigned int count = 1)
    : buffer(new UniformBuffer(data, count))
  {
  }

  // デストラクタ
  virtual ~Uniform()
  {
  }

  // ユニフォームバッファオブジェクトにデータを格納する
  //   data: uniform ブロックに格納するデータ
  //   start: データを格納する uniform ブロックの先頭位置
  //   count: データを格納する uniform ブロックの数
  void set(const T *data, unsigned int start = 0, unsigned int count = 1) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->ubo);
    for (unsigned int i = 0; i < count; ++i)
    {
      glBufferSubData(GL_UNIFORM_BUFFER, (start + i) * buffer->blocksize,
        sizeof (T), data + i);
    }
  }

  // このユニフォームバッファオブジェクトを使用する
  //   bp: 結合ポイント
  //   i: 結合する uniform ブロックの位置
  void select(GLuint bp, unsigned int i = 0) const
  {
    // 結合ポイントにユニフォームバッファオブジェクトを結合する
    glBindBufferRange(GL_UNIFORM_BUFFER, bp,
      buffer->ubo, i * buffer->blocksize, sizeof (T));
  }
};
