#include <krEngine/rendering/implementation/extractionBuffer.h>

kr::ExtractionBuffer::ExtractionBuffer(ezAllocatorBase* pAllocator) :
  m_pAllocator(pAllocator)
{
  grow(1024);
}

kr::ExtractionBuffer::~ExtractionBuffer()
{
  m_pAllocator->Deallocate(m_data);
  m_current = m_end = nullptr;
}

void* kr::ExtractionBuffer::allocate(size_t bytes, size_t alignment)
{
  EZ_ASSERT(m_mode == Mode::WriteOnly, "Invalid operation in this mode.");

  // Check if we need to grow.
  // =========================
  auto cap = getByteCapacity();
  while (getNumAllocatedBytes() + bytes > cap)
  {
    cap *= 2;
  }

  if (cap > getByteCapacity())
  {
    grow(cap);
  }

  // Do the allocation
  // =================

  // Align m_current
  /// \todo Fix this unnecessarily slow alignment adjustment.
  while ((size_t)m_current % alignment != 0)
    ++m_current;

  // Assign the aligned pointer.
  auto result = m_current;

  // Advance the aligned m_current.
  m_current += bytes;

  EZ_CHECK_ALIGNMENT(result, alignment);
  return result;
}

void kr::ExtractionBuffer::grow(size_t newCapacity)
{
  EZ_ASSERT(m_growthAllowed && m_mode == Mode::WriteOnly,
            "Invalid operation: Growing is not allowed.");

  auto newMem = (ezUByte*)m_pAllocator->Allocate(newCapacity,
                                                EZ_ALIGNMENT_OF(ezUByte));
  auto currentlyAllocatedBytes = getNumAllocatedBytes();
  if (currentlyAllocatedBytes > 0)
  {
    // If something is currently allocated, copy it over to the new buffer.
    memcpy(newMem, m_data, currentlyAllocatedBytes);
  }

  // Free the previously allocated memory.
  if (m_data)
    m_pAllocator->Deallocate(m_data);

  // Update the pointers.
  m_data = newMem;
  m_current = m_data + currentlyAllocatedBytes;
  m_end = m_data + newCapacity;
}
