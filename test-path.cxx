#include <filesystem>
#include <stdio.h>

int wmain(int argc, wchar_t **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        std::filesystem::path p{argv[i]};
        wprintf(L"native          : %ls\n", p.native().c_str());
        p.make_preferred();
        wprintf(L"preferred_native: %ls\n", p.native().c_str());
        wprintf(L"generic_wstring : %ls\n", p.generic_wstring().c_str());
    }
}
