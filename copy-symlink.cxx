#include <filesystem>
#include <stdio.h>

namespace fs = std::filesystem;

int wmain(int argc, wchar_t **argv)
{
  if (argc < 3)
  {
    fwprintf(stderr, L"Usage: %ls [src] [dst]\n", argv[0]);
    return 1;
  }

  std::error_code ec;
  fs::copy_symlink(argv[1], argv[2], ec);

  if (ec)
  {
    fwprintf(stderr, L"Failed to copy_symlink(%ls -> %ls) [ec = %hs (0x%x)]\n", argv[1], argv[2], ec.message().c_str(), ec.value());
    return 1;
  }

  wprintf(L"copy_symlink(%ls -> %ls)\n", argv[1], argv[2]);
  return 0;
}
