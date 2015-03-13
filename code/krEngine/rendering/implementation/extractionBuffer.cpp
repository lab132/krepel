#include <krEngine/pch.h>
#include <krEngine/rendering/implementation/extractionBuffer.h>

kr::ExtractionBuffer::ExtractionBuffer(ezAllocatorBase* pAllocator) :
  m_pAllocator(pAllocator)
{
  grow(1024);
}

kr::ExtractionBuffer::~ExtractionBuffer()
{
  m_pAllocator->Deallocate(m_data);
  m_current = m_max = nullptr;
}

void* kr::ExtractionBuffer::allocate(size_t alignedSize)
{
  EZ_ASSERT_RELEASE(m_mode == Mode::WriteOnly, "Invalid operation in this mode.");

  // Check if we need to grow.
  // =========================
  auto cap = getByteCapacity();
  while (getNumAllocatedBytes() + alignedSize > cap)
  {
    cap *= 2;
  }

  if (cap > getByteCapacity())
  {
    grow(cap);
  }

  // Do the allocation
  // =================

  auto result = m_current;

  m_current += alignedSize;

  return result;
}

void kr::ExtractionBuffer::grow(size_t newCapacity)
{
  EZ_ASSERT_RELEASE(m_growthAllowed && m_mode == Mode::WriteOnly,
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
  m_max = m_data + newCapacity;
}
