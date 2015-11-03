#pragma once

namespace kr
{
  struct LoopListener;

  class KR_ENGINE_API GameLoop
  {
  public: // *** Construction
    GameLoop() = default;

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

  private: // *** Internal
    struct Callback
    {
      ezString name;
      ezDelegate<void()> func;
    };

    Callback* getCallback(ezStringView name);

  private: // *** Data
    ezDynamicArray<Callback> m_callbacks;
  };

  /// \brief Static class to register game loops with a name.
  ///
  /// \todo Dependency management. Something like \c depend(GameLoop* pLoop, GameLoop* pDependency);. Fail for cyclic deps.
  class KR_ENGINE_API GlobalGameLoopRegistry
  {
    GlobalGameLoopRegistry() = delete;
  public:

    /// \brief Add a named gameloop to the global gameloop registry.
    /// \return \c EZ_SUCCESS if the names gameloop did not exist yet, \c EZ_FAILURE otherwise.
    static ezResult add(ezStringView name, GameLoop* pLoop, ezLogInterface* pLogInterface = nullptr);

    /// \brief Get a registered gameloop instance by name.
    /// \return \c nullptr if the named gameloop does not exist.
    static GameLoop* get(ezStringView name, ezLogInterface* pLogInterface = nullptr);

    /// \brief Remove a registered gameloop by pointer.
    ///
    /// You can also remove a registered gameloop by name with the overloaded version of remove().
    ///
    /// \return \c EZ_SUCCESS if the gameloop could successfully be removed, \c EZ_FAILURE if \p pLoop is null or not registered.
    ///
    /// \see remove()
    static ezResult remove(GameLoop* pLoop, ezLogInterface* pLogInterface = nullptr);

    /// \brief Remove a registered gameloop by name.
    /// \see remove()
    static ezResult remove(ezStringView name, ezLogInterface* pLogInterface = nullptr) { return remove(get(name, pLogInterface), pLogInterface); }

    static void tick(ezLogInterface* pLogInterface = nullptr);
  };
}
