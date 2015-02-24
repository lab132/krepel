
static ezResult destroyOpenGLContext(kr::WindowImpl& window)
{
  EZ_LOG_BLOCK("Renderer Destroying OpenGL Context");

  auto hWnd = window.m_handler.GetNativeWindowHandle();

  if (!window.m_hRC && !window.m_hDC)
    return EZ_SUCCESS;

  if (window.m_hRC)
  {
    ezLog::Dev("Destroying the Rendering Context.");

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(window.m_hRC);
    window.m_hRC = nullptr;
  }

  if (window.m_hDC)
  {
    ezLog::Dev("Destroying the Device Context.");

    ReleaseDC(hWnd, window.m_hDC);
    window.m_hDC = nullptr;
  }

  ezLog::Success("OpenGL graphics context is destroyed.");

  return EZ_SUCCESS;
}

static ezResult createOpenGLContext(kr::WindowImpl& window)
{
  EZ_LOG_BLOCK("Renderer Creating OpenGL Context");

  auto hWnd = window.m_handler.GetNativeWindowHandle();

  int iColorBits = 24;
  int iDepthBits = 24;
  int iBPC = 8;

  DWORD flags = PFD_DRAW_TO_WINDOW
    | PFD_SUPPORT_OPENGL
    | PFD_DOUBLEBUFFER
    | PFD_SWAP_EXCHANGE;
  // clang-format off
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof (PIXELFORMATDESCRIPTOR),
    1, // Version
    flags,
    PFD_TYPE_RGBA, // Pixeltype
    iColorBits, // Color Bits
    iBPC, 0, iBPC, 0, iBPC, 0, iBPC, 0,// Red Bits / Red Shift, Green Bits / Shift, Blue Bits / Shift, Alpha Bits / Shift
    0, 0, 0, 0, 0, // Accum Bits (total), Accum Bits Red, Green, Blue, Alpha
    iDepthBits, 8, // Depth, Stencil Bits
    0, // Aux Buffers
    PFD_MAIN_PLANE, // Layer Type (ignored)
    0, 0, 0, 0 // ignored deprecated flags
  };
  // clang-format on

  window.m_hDC = GetDC(hWnd);

  if (window.m_hDC == nullptr)
  {
    ezLog::Error("Could not retrieve the Window DC");
    goto failure;
  }

  int iPixelformat = ChoosePixelFormat(window.m_hDC, &pfd);
  if (iPixelformat == 0)
  {
    ezLog::Error("ChoosePixelFormat failed.");
    goto failure;
  }

  if (!SetPixelFormat(window.m_hDC, iPixelformat, &pfd))
  {
    ezLog::Error("SetPixelFormat failed.");
    goto failure;
  }

  window.m_hRC = wglCreateContext(window.m_hDC);
  if (window.m_hRC == nullptr)
  {
    ezLog::Error("wglCreateContext failed.");
    goto failure;
  }

  if (!wglMakeCurrent(window.m_hDC, window.m_hRC))
  {
    ezLog::Error("wglMakeCurrent failed.");
    goto failure;
  }

  SetFocus(hWnd);
  SetForegroundWindow(hWnd);

  ezLog::Success("OpenGL (%s) graphics context is initialized.",
                 glGetString(GL_VERSION));

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    ezLog::Error("Failed to initialize GLEW");
    goto failure;
  }

  ezLog::Success("Initialized GLEW version %s", glewGetString(GLEW_VERSION));

  return EZ_SUCCESS;

failure:
  ezLog::Error("Failed to initialize the graphics context.");

  destroyOpenGLContext(window);
  return EZ_FAILURE;
}
