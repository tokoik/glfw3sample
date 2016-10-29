#include <cstdlib>
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
#include "SolidShapeIndex.h"

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

// 面ごとに色を変えた六面体の頂点属性
constexpr Object::Vertex solidCubeVertex[] =
{
  // 左
  { -1.0f, -1.0f, -1.0f,  0.1f,  0.8f,  0.1f },
  { -1.0f, -1.0f,  1.0f,  0.1f,  0.8f,  0.1f },
  { -1.0f,  1.0f,  1.0f,  0.1f,  0.8f,  0.1f },
  { -1.0f,  1.0f, -1.0f,  0.1f,  0.8f,  0.1f },

  // 裏
  {  1.0f, -1.0f, -1.0f,  0.8f,  0.1f,  0.8f },
  { -1.0f, -1.0f, -1.0f,  0.8f,  0.1f,  0.8f },
  { -1.0f,  1.0f, -1.0f,  0.8f,  0.1f,  0.8f },
  {  1.0f,  1.0f, -1.0f,  0.8f,  0.1f,  0.8f },

  // 下
  { -1.0f, -1.0f, -1.0f,  0.1f,  0.8f,  0.8f },
  {  1.0f, -1.0f, -1.0f,  0.1f,  0.8f,  0.8f },
  {  1.0f, -1.0f,  1.0f,  0.1f,  0.8f,  0.8f },
  { -1.0f, -1.0f,  1.0f,  0.1f,  0.8f,  0.8f },

  // 右
  {  1.0f, -1.0f,  1.0f,  0.1f,  0.1f,  0.8f },
  {  1.0f, -1.0f, -1.0f,  0.1f,  0.1f,  0.8f },
  {  1.0f,  1.0f, -1.0f,  0.1f,  0.1f,  0.8f },
  {  1.0f,  1.0f,  1.0f,  0.1f,  0.1f,  0.8f },

  // 上
  { -1.0f,  1.0f, -1.0f,  0.8f,  0.1f,  0.1f },
  { -1.0f,  1.0f,  1.0f,  0.8f,  0.1f,  0.1f },
  {  1.0f,  1.0f,  1.0f,  0.8f,  0.1f,  0.1f },
  {  1.0f,  1.0f, -1.0f,  0.8f,  0.1f,  0.1f },

  // 前
  { -1.0f, -1.0f,  1.0f,  0.8f,  0.8f,  0.1f },
  {  1.0f, -1.0f,  1.0f,  0.8f,  0.8f,  0.1f },
  {  1.0f,  1.0f,  1.0f,  0.8f,  0.8f,  0.1f },
  { -1.0f,  1.0f,  1.0f,  0.8f,  0.8f,  0.1f }
};

// 六面体の面を塗りつぶす三角形の頂点のインデックス
constexpr GLuint solidCubeIndex[] =
{
   0,  1,  2,  0,  2,  3, // 左
   4,  5,  6,  4,  6,  7, // 裏
   8,  9, 10,  8, 10, 11, // 下
  12, 13, 14, 12, 14, 15, // 右
  16, 17, 18, 16, 18, 19, // 上
  20, 21, 22, 20, 22, 23  // 前
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
  std::unique_ptr<const Shape> shape(new SolidShapeIndex(3, 24, solidCubeVertex, 36, solidCubeIndex));

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
