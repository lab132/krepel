
static ezStringBuilder cwdHelper()
{
  auto bufferSize = GetCurrentDirectory(0, nullptr);
  auto buffer = new TCHAR[bufferSize];
  KR_ON_SCOPE_EXIT{ delete[] buffer; };
  if(GetCurrentDirectory(bufferSize, buffer) != bufferSize)
  {
    ezLog::Error("Error calling GetCurrentDirectory.");
    return ezStringBuilder();
  }
  ezStringBuilder path(buffer);
  path.MakeCleanPath();
  return path;
}

ezStringView kr::cwd()
{
  static ezStringBuilder path = cwdHelper();
  return path;
}
