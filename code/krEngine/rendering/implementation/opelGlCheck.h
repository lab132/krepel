#pragma once

// Note: This is taken and modified from the SFML project.
//       See http://www.sfml-dev.org/

namespace krInternal
{
  void glCheckForError(const char* expression, const char* fileName, size_t line);
}

#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  #define glCheckLastError()                                                             \
    do                                                                                   \
    {                                                                                    \
      ::krInternal::glCheckForError(EZ_STRINGIZE(call), EZ_SOURCE_FILE, EZ_SOURCE_LINE); \
    } while (false)

  #define glCheck(call)   \
    do                    \
    {                     \
      call;               \
      glCheckLastError(); \
    } while (false)

#else
  #define glCheckLastError()
  #define glCheck(call) (call)
#endif // EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
