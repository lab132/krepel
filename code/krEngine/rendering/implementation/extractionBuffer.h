#pragma once
#include <krEngine/rendering/implementation/extractionDetails.h>

namespace kr
{
  class ExtractionBuffer
  {
  public: // ** Public Types
    struct Mode
    {
      enum Enum
      {
        ReadOnly,
        WriteOnly,
      };
    };
  public: // *** Construction
    ExtractionBuffer(ezAllocatorBase* pAllocator);
    ~ExtractionBuffer();

  public: // *** Public API

    /// \brief Allocates data of the given type.
    template<typename T>
    T* allocate()
    {
      const auto type = static_cast<ExtractionDataType>(T::Type);

      EZ_CHECK_AT_COMPILETIME((std::is_base_of<ExtractionData, T>::value));
      EZ_CHECK_AT_COMPILETIME(type != ExtractionDataType::Invalid);

      const auto alignment = EZ_ALIGNMENT_OF(T);
      const auto alignedSize = ezMemoryUtils::AlignSize(sizeof(T), alignment);
      auto result =  new (allocate(alignedSize)) T;
      EZ_CHECK_ALIGNMENT(result, alignment);
      result->type = type;
      result->byteCount = alignedSize;
      return static_cast<T*>(result);
    }

    /// \brief Reset the current allocation pointer.
    /// \note Does not do any (re)allocations.
    void reset() { m_current = m_data; }

    /// \brief Number of bytes currently allocated.
    size_t getNumAllocatedBytes() const { return m_current - m_data; }

    /// \brief Number of bytes currently available without having to grow.
    size_t getByteCapacity() const { return m_max - m_data; }

    void setGrowthAllowed(bool allowed) { m_growthAllowed = allowed; }

    void setMode(Mode::Enum mode) { m_mode = mode; }

  public: // *** Friends & Algorithms
    friend void swap(ExtractionBuffer*& readOnly, ExtractionBuffer*& writeOnly)
    {
      kr::swap(readOnly, writeOnly);
      readOnly->m_mode = Mode::ReadOnly;
      writeOnly->m_mode = Mode::WriteOnly;
    }

    friend ezUByte* begin(ExtractionBuffer& e) { return e.m_data; }
    friend ezUByte* end(ExtractionBuffer& e) { return e.m_current; }

  private: // *** Internal Functions
    void* allocate(size_t alignedSize);
    void grow(size_t newCapacity);

  private: // *** Internal Data
    ezAllocatorBase* m_pAllocator;
    ezUByte* m_data = nullptr;
    ezUByte* m_current = nullptr;
    ezUByte* m_max = nullptr;
    Mode::Enum m_mode = Mode::WriteOnly;
    bool m_growthAllowed = true;

  private:
    EZ_DISALLOW_COPY_AND_ASSIGN(ExtractionBuffer);
  };
}
