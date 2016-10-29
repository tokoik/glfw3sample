﻿#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Matrix.h"
#include "Shape.h"
#include "ShapeIndex.h"

// シェーダオブジェクトのコンパイル結果を表示する
//   shader: シェーダオブジェクト名
//   str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
  // コンパイル結果を取得する
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

  // シェーダのコンパイル時のログの長さを取得する
  GLsizei bufSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1)
  {
    // シェーダのコンパイル時のログの内容を取得する
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  return static_cast<GLboolean>(status);
}

// プログラムオブジェクトのリンク結果を表示する
//   program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program)
{
  // リンク結果を取得する
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;

  // シェーダのリンク時のログの長さを取得する
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1)
  {
    // シェーダのリンク時のログの内容を取得する
    std::vector<GLchar> infoLog(bufSize);
    GLsizei length;
    glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
  }

  return static_cast<GLboolean>(status);
}

// プログラムオブジェクトを作成する
//   vsrc: バーテックスシェーダのソースプログラムの文字列
//   fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc)
{
  // 空のプログラムオブジェクトを作成する
  const GLuint program(glCreateProgram());

  if (vsrc != NULL)
  {
    // バーテックスシェーダのシェーダオブジェクトを作成する
    const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
    glShaderSource(vobj, 1, &vsrc, NULL);
    glCompileShader(vobj);

    // バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
    if (printShaderInfoLog(vobj, "vertex shader"))
      glAttachShader(program, vobj);
    glDeleteShader(vobj);
  }

  if (fsrc != NULL)
  {
    // フラグメントシェーダのシェーダオブジェクトを作成する
    const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(fobj, 1, &fsrc, NULL);
    glCompileShader(fobj);

    // フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
    if (printShaderInfoLog(fobj, "fragment shader"))
      glAttachShader(program, fobj);
    glDeleteShader(fobj);
  }

  // プログラムオブジェクトをリンクする
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "color");
  glBindFragDataLocation(program, 0, "fragment");
  glLinkProgram(program);

  // 作成したプログラムオブジェクトを返す
  if (printProgramInfoLog(program))
    return program;

  // プログラムオブジェクトが作成できなければ 0 を返す
  glDeleteProgram(program);
  return 0;
}

// シェーダのソースファイルを読み込む
//   name: シェーダのソースファイル名
//   buffer: 読み込んだソースファイルのテキスト
bool readShaderSource(const char *name, std::vector<GLchar> &buffer)
{
  // ファイル名が NULL だった
  if (name == NULL) return false;

  // ソースファイルを開く
  std::ifstream file(name, std::ios::binary);
  if (file.fail())
  {
    // 開けなかった
    std::cerr << "Error: Can't open source file: " << name << std::endl;
    return false;
  }

  // ファイルの末尾に移動し現在位置（＝ファイルサイズ）を得る
  file.seekg(0L, std::ios::end);
  GLsizei length = static_cast<GLsizei>(file.tellg());

  // ファイルサイズのメモリを確保
  buffer.resize(length + 1);

  // ファイルを先頭から読み込む
  file.seekg(0L, std::ios::beg);
  file.read(buffer.data(), length);
  buffer[length] = '\0';

  if (file.fail())
  {
    // うまく読み込めなかった
    std::cerr << "Error: Could not read souce file: " << name << std::endl;
    file.close();
    return false;
  }

  // 読み込み成功
  file.close();
  return true;
}

// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
//   vert: バーテックスシェーダのソースファイル名
//   frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char *vert, const char *frag)
{
  // シェーダのソースファイルを読み込む
  std::vector<GLchar> vsrc;
  const bool vstat(readShaderSource(vert, vsrc));
  std::vector<GLchar> fsrc;
  const bool fstat(readShaderSource(frag, fsrc));

  // プログラムオブジェクトを作成する
  return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// 六面体の頂点の位置
constexpr Object::Vertex cubeVertex[] =
{
  { -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f },  // (0)
  { -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.8f },  // (1)
  { -1.0f,  1.0f,  1.0f,  0.0f,  0.8f,  0.0f },  // (2)
  { -1.0f,  1.0f, -1.0f,  0.0f,  0.8f,  0.8f },  // (3)
  {  1.0f,  1.0f, -1.0f,  0.8f,  0.0f,  0.0f },  // (4)
  {  1.0f, -1.0f, -1.0f,  0.8f,  0.0f,  0.8f },  // (5)
  {  1.0f, -1.0f,  1.0f,  0.8f,  0.8f,  0.0f },  // (6)
  {  1.0f,  1.0f,  1.0f,  0.8f,  0.8f,  0.8f }   // (7)
};

// 六面体の稜線の両端点のインデックス
constexpr GLuint wireCubeIndex[] =
{
  1, 0, // (a)
  2, 7, // (b)
  3, 0, // (c)
  4, 7, // (d)
  5, 0, // (e)
  6, 7, // (f)
  1, 2, // (g)
  2, 3, // (h)
  3, 4, // (i)
  4, 5, // (j)
  5, 6, // (k)
  6, 1  // (l)
};

int main()
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // 初期化に失敗した
    std::cerr << "Can't initialize GLFW" << std::endl;
    return 1;
  }

  // プログラム終了時の処理を登録する
  atexit(glfwTerminate);

  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // ウィンドウを作成する
  Window window;

  // 背景色を指定する
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  // プログラムオブジェクトを作成する
  const GLuint program(loadProgram("point.vert", "point.frag"));

  // uniform 変数の場所を取得する
  const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
  const GLint projectionLoc(glGetUniformLocation(program, "projection"));

  // 図形データを作成する
  std::unique_ptr<const Shape> shape(new ShapeIndex(3, 8, cubeVertex, 24, wireCubeIndex));

  // ウィンドウが開いている間繰り返す
  while (window)
  {
    // ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // シェーダプログラムの使用開始
    glUseProgram(program);

    // 透視投影変換行列を求める
    const GLfloat *const size(window.getSize());
    const GLfloat fovy(window.getScale() * 0.01f);
    const GLfloat aspect(size[0] / size[1]);
    const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));

    // モデル変換行列を求める
    const GLfloat *const location(window.getLocation());
    const Matrix model(Matrix::translate(location[0], location[1], 0.0f));

    // ビュー変換行列を求める
    const Matrix view(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

    // モデルビュー変換行列を求める
    const Matrix modelview(view * model);

    // uniform 変数に値を設定する
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());

    // 図形を描画する
    shape->draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
