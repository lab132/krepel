#pragma once
#include <krEngine/rendering/drawableBase.h>

namespace kr
{
  class RenderExtractionContext
  {
  public:
    RenderExtractionContext(ezArrayPtr<ezUInt8> buffer) :
      m_start(buffer.GetPtr()),
      m_current(m_start),
      m_end(m_start + buffer.GetCount())
    {
    }

    template<typename Type>
    Type* allocate()
    {
      // check if the given Type is valid
      static_assert(std::is_convertible<Type, DrawableBase>::value, "Invalid type.");
      return static_cast<Type*>(allocate(sizeof(Type), EZ_ALIGNMENT_OF(Type)));
    }

    size_t getAllocatedSize() const
    {
      return m_current - m_start;
    }

  private:
    void* allocate(size_t uiSize, size_t uiAlign)
    {
      auto mem = m_current;
      m_current += uiSize;
      // TODO use uiAlign?

      return (void*)mem;
    }

  private:
    ezUInt8* m_start;
    ezUInt8* m_current;
    ezUInt8* m_end;
  };

  class KR_ENGINE_API RenderExtractor
  {
  public:
    using Event = ezEvent<RenderExtractionContext&>;
    using Listener = Event::Handler;

    static void extract();

    static void addListener(Listener listener)
    {
      s_extractionEvent.AddEventHandler(listener);
    }

    static void removeListener(Listener listener)
    {
      s_extractionEvent.RemoveEventHandler(listener);
    }

  protected:
  private:
    enum { BUFFER_SIZE = 1024 };

    static Event s_extractionEvent;
    static byte* s_buffers[2];
    static byte* s_readBuffer;
    static byte* s_writeBuffer;
  };
}
