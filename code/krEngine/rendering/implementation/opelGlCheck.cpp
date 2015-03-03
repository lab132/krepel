#include <krEngine/pch.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

// Note: This is taken and modified from the SFML project.
//       See http://www.sfml-dev.org/

void krInternal::glCheckForError(const char* expression, const char* fileName, size_t line)
{
  // Get the last error.
  GLenum errorCode = glGetError();

  // No need to do anything if no error occurred.
  if (errorCode == GL_NO_ERROR)
    return;

  EZ_LOG_BLOCK("OpenGL Error", expression);

  // Log the file and line in which the error occurred
  // =================================================
  ezStringBuilder message = fileName;
  message.MakeCleanPath();
  message.MakePathSeparatorsNative();

  message.AppendFormat("(%u):", line);
  ezLog::Warning("%s", message.GetData());

  message.Clear();

  // Log the OpenGL error
  // ====================

  switch (errorCode)
  {
  case GL_INVALID_ENUM:
    ezLog::Warning("%s", "GL_INVALID_ENUM: An unacceptable value has been specified for an enumerated argument.");
    break;
  case GL_INVALID_VALUE:
    ezLog::Warning("%s", "GL_INVALID_VALUE: A numeric argument is out of range.");
    break;
  case GL_INVALID_OPERATION:
    ezLog::Warning("%s", "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.");
    break;
  case GL_STACK_OVERFLOW:
    ezLog::Warning("%s", "GL_STACK_OVERFLOW: This command would cause a stack overflow.");
    break;
  case GL_STACK_UNDERFLOW:
    ezLog::Warning("%s", "GL_STACK_UNDERFLOW: This command would cause a stack underflow.");
    break;
  case GL_OUT_OF_MEMORY:
    ezLog::Warning("%s", "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.");
    break;
  default:
    ezLog::Warning("Unkown error.");
    break;
  }

  EZ_DEBUG_BREAK;
}
