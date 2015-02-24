#pragma once

class ezImage;

namespace kr
{
  namespace Texture
  {
    /// \brief A handle to an actual texture instance.
    ///
    /// This is used with all functions to access the actual texture data.
    struct KR_ENGINE_API Handle
    {
      /// \brief The internal reference to the actual texture.
      ezUInt32 hash : 8;
      ezUInt32 index : 24;

      Handle() : hash(-1), index(-1) {}

      bool isValid() const;

      bool operator ==(const Handle& rhs) const
      {
        return index == rhs.index
            && hash == rhs.hash;
      }
      bool operator !=(const Handle& rhs) const { return !(*this == rhs); }
    };

    EZ_CHECK_AT_COMPILETIME(sizeof(Handle) == sizeof(ezUInt32));

    /// \brief Loads a texture from the filesystem with the given \a filename.
    KR_ENGINE_API Handle load(const char* filename);

    /// \brief Unloads a texture if there are no more references to it and invalidates the given handle.
    KR_ENGINE_API void unload(Handle& hTex);

    KR_ENGINE_API ezUInt32 getWidth(Handle hTex);
    KR_ENGINE_API ezUInt32 getHeight(Handle hTex);

    /// \brief Gets the name of the texture.
    ///
    /// If the texture was loaded from a file, this will be the filename.
    /// Else the name will be the given string.
    KR_ENGINE_API const ezString64& getName(Handle hTex);

    /// \brief Get the underlying image data of the texture.
    KR_ENGINE_API const ezImage& getImage(Handle hTex);
  }
}
