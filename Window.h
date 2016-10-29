#pragma once

//
// ウィンドウ関連の処理
//
class Window
{
  // ウィンドウの識別子
  GLFWwindow *const window;

  // ウィンドウのサイズ
  GLfloat size[2];

  // ワールド座標系に対するデバイス座標系の拡大率
  GLfloat scale;

  // 図形の正規化デバイス座標系上での位置
  GLfloat location[2];

public:

  // コンストラクタ
  Window(int width = 640, int height = 480, const char *title = "Hello!")
    : window(glfwCreateWindow(width, height, title, NULL, NULL))
    , scale(100.0f), location{ 0, 0 }
  {
    if (window == NULL)
    {
      // ウィンドウが作成できなかった
      std::cerr << "Can't create GLFW window." << std::endl;
      exit(1);
    }

    // 現在のウィンドウを処理対象にする
    glfwMakeContextCurrent(window);

    // GLEW を初期化する
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
      // GLEW の初期化に失敗した
      std::cerr << "Can't initialize GLEW" << std::endl;
      exit(1);
    }

    // 作成したウィンドウに対する設定
    glfwSwapInterval(1);

    // ウィンドウのサイズ変更時に呼び出す処理の登録
    glfwSetWindowSizeCallback(window, resize);

    // このインスタンスの this ポインタを記録しておく
    glfwSetWindowUserPointer(window, this);

    // 開いたウィンドウの初期設定
    resize(window, width, height);
  }

  // デストラクタ
  virtual ~Window()
  {
    // ウィンドウを破棄する
    glfwDestroyWindow(window);
  }

  // 描画ループの継続判定
  explicit operator bool()
  {
    // イベントを取り出す
    glfwWaitEvents();

    // マウスカーソルの位置を取得する
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // マウスカーソルの正規化デバイス座標系上での位置を求める
    location[0] = static_cast<GLfloat>(x) * 2.0f / size[0] - 1.0f;
    location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / size[1];

    // ウィンドウを閉じる必要がなければ true を返す
    return !glfwWindowShouldClose(window);
  }

  // ダブルバッファリング
  void swapBuffers() const
  {
    // カラーバッファを入れ替える
    glfwSwapBuffers(window);
  }

  // ウィンドウのサイズ変更時の処理
  static void resize(GLFWwindow *window, int width, int height)
  {
    // フレームバッファの大きさを得る
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    
    // フレームバッファ全体をビューポートにする
    glViewport(0, 0, fbWidth, fbHeight);

    // このインスタンスの this ポインタを得る
    Window *const
      instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance != NULL)
    {
      // 開いたウィンドウのサイズを保存する
      instance->size[0] = static_cast<GLfloat>(width);
      instance->size[1] = static_cast<GLfloat>(height);
    }
  }

  // ウィンドウのサイズを取り出す
  const GLfloat *getSize() const { return size; }

  // ワールド座標系に対するデバイス座標系の拡大率を取り出す
  GLfloat getScale() const { return scale; }

  // 位置を取り出す
  const GLfloat *getLocation() const { return location; }
};
