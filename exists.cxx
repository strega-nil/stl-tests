#include <filesystem>
#include <stdio.h>

namespace fs = std::filesystem;

int wmain(int argc, wchar_t **argv)
{
    std::error_code ec;
    for (int i = 1; i < argc; ++i)
    {
        wprintf(L"exists(%ls): %ls\n", argv[i], fs::exists(argv[i], ec) ? L"true" : L"false");
        if (ec)
        {
            wprintf(L"  error: 0x%lX\n", ec.value());
        }
    }
}
