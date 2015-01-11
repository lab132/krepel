#include "krEngine/pch.h"
#include "krEngine/rendering/renderExtractor.h"

kr::RenderExtractor::Event kr::RenderExtractor::s_extractionEvent;
ezUInt8* kr::RenderExtractor::s_buffers[2];
ezUInt8* kr::RenderExtractor::s_readBuffer;
ezUInt8* kr::RenderExtractor::s_writeBuffer;

void kr::RenderExtractor::extract()
{
  RenderExtractionContext context(ezArrayPtr<ezUInt8>(s_writeBuffer, BUFFER_SIZE));
  // trigger extraction event and collect draw data.
  s_extractionEvent.Broadcast(context);

  /// \todo wait for renderer to finish
  swap(s_readBuffer, s_writeBuffer);
  /// \todo signal renderer to continue rendering
}
