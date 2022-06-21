#include <filesystem>
#include <stdio.h>

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3)
    {
        fwprintf(stderr, L"Usage: %ls [src] [dst]\n", argv[0]);
        return 1;
    }

    std::error_code ec;
    std::filesystem::rename(argv[1], argv[2], ec);

    if (ec)
    {
        fwprintf(stderr, L"Failed to rename %ls -> %ls: 0x%x %hs\n", argv[1], argv[2], ec.value(), ec.message().c_str());
        return 1;
    }

    wprintf(L"Successfully renamed %ls -> %ls\n", argv[1], argv[2]);
    return 0;
}
