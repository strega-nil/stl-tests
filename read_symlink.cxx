#include <filesystem>
#include <stdio.h>

namespace fs = std::filesystem;

const char *to_string(const fs::file_status &status)
{
  switch (status.type())
  {
  case fs::file_type::block:
    return "block";
  case fs::file_type::character:
    return "character";
  case fs::file_type::directory:
    return "directory";
  case fs::file_type::fifo:
    return "fifo";
  case fs::file_type::junction:
    return "junction";
  case fs::file_type::regular:
    return "regular";
  case fs::file_type::socket:
    return "socket";
  case fs::file_type::symlink:
    return "symlink";
  case fs::file_type::not_found:
    return "not found";
  case fs::file_type::none:
  case fs::file_type::unknown:
  default:
    return "unknown";
  }
}

int wmain(int argc, wchar_t **argv)
{
  if (argc < 2)
  {
    fwprintf(stderr, L"Usage: %ls [file]\n", argv[0]);
    return 1;
  }

  fs::path file = argv[1];

  std::error_code ec;

  auto points_to = fs::read_symlink(file, ec);
  wprintf(L"read_symlink('%ls'): %ls [ec = %hs (0x%x)]\n",
          file.c_str(),
          points_to.c_str(),
          ec.message().c_str(),
          ec.value());

  auto status = fs::symlink_status(file, ec);
  wprintf(L"status('%ls'): %hs [ec = %hs (0x%x)]\n",
          file.c_str(),
          to_string(status),
          ec.message().c_str(),
          ec.value());
  auto is_symlink = fs::is_symlink(status);
  wprintf(L"    is_symlink? %ls\n", is_symlink ? L"true" : L"false");

  return 0;
}
