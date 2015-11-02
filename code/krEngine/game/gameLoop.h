#pragma once

namespace kr
{
  struct LoopListener;

  class KR_ENGINE_API GameLoop
  {
  public: // *** Built-In Channels
    // These are created lazily.

    static GameLoop* preMainChannel();
    static GameLoop* mainChannel();
    static GameLoop* postMainChannel();

    static GameLoop* preRenderingChannel();
    static GameLoop* renderingChannel();
    static GameLoop* postRenderingChannel();

  public: // *** Construction
    GameLoop() = default;
    GameLoop(ezStringView name) : m_name{ name } {}

  public: // *** Runtime

    /// \brief Add a named callback function to this game loop.
    ///
    /// Will not overwrite an existing callback with the name \p callbackName.
    ///
    /// \return Returns \c EZ_SUCCESS if no other callback with the same name existed yet, else \c EZ_FAILURE.
    /// \see overwriteCallback()
    ezResult addCallback(ezStringView callbackName, ezDelegate<void()> callback);

    /// \brief Overwrite or add a named callback function to this game loop.
    ///
    /// Will add the callback if it didn't exist yet.
    ///
    /// \see addCallback()
    void updateCallback(ezStringView callbackName, ezDelegate<void()> callback);

    /// \brief Remove a named callback from this game loop.
    ///
    /// If the callback existed, will return EZ_SUCCESS.
    /// Otherwise EZ_FAILURE is returned and nothing else is done.
    ezResult removeCallback(ezStringView callbackName);

    void tick(ezLogInterface* pLogInterface = nullptr);

    void setName(ezStringView name) { m_name = name; }
    ezStringView name() const { return m_name; }

  private: // *** Internal
    struct Callback
    {
      ezString name;
      ezDelegate<void()> func;
    };

    Callback* getCallback(ezStringView name);

  private: // *** Data
    ezString m_name{ "<anon>" };
    ezDynamicArray<Callback> m_callbacks;
  };

  class KR_ENGINE_API GlobalGameLoopRegistry
  {
    GlobalGameLoopRegistry() = delete;
  public:

    static ezResult add(GameLoop* pLoop);
    static ezResult remove(GameLoop* pLoop);

    // TODO Dependency management.

    static void tick();
  };
}
