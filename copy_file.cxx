#include <filesystem>
#include <stdio.h>

namespace fs = std::filesystem;

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3)
    {
        fwprintf(stderr, L"Not enough arguments to %ls; got %d, expected at least 3\n", argv[0], argc - 1);
        return 1;
    }

    const wchar_t *src = nullptr;
    const wchar_t *dest = nullptr;

    auto opts = fs::copy_options::none;
    bool ignore_flags = false; // after --, all arguments are treated as files
    for (int i = 1; i < argc; ++i)
    {
        const wchar_t *current_arg = argv[i];
        if (!ignore_flags && current_arg[0] == L'-')
        {
            if (current_arg[1] != L'-')
            {
                fwprintf(stderr, L"%ls does not support short options\n", argv[0]);
                return 1;
            }

            // flag
            if (wcscmp(current_arg, L"--") == 0)
            {
                ignore_flags = true;
                continue;
            }
            if (wcscmp(current_arg, L"--skip-existing") == 0)
            {
                opts = fs::copy_options::skip_existing;
                continue;
            }
            if (wcscmp(current_arg, L"--overwrite-existing") == 0)
            {
                opts = fs::copy_options::overwrite_existing;
                continue;
            }
            if (wcscmp(current_arg, L"--update-existing") == 0)
            {
                opts = fs::copy_options::update_existing;
                continue;
            }
            fwprintf(stderr, L"Unrecognized option %ls\n", current_arg);
            return 1;
        }
        if (!src)
        {
            src = current_arg;
            continue;
        }
        if (!dest)
        {
            dest = current_arg;
            continue;
        }
        fwprintf(stderr, L"Too many arguments to %ls; expected only two files\n", argv[0]);
        return 1;
    }

    if (!dest || !src)
    {
        fwprintf(stderr, L"Not enough arguments to %ls; need both a dest and a src\n", argv[0]);
        return 1;
    }

    std::error_code ec;
    wprintf(L"copy %ls -> %ls - %ls\n", src, dest, fs::copy_file(src, dest, opts, ec) ? L"copied" : L"skipped");
    if (ec)
    {
        fwprintf(stderr, L"  copy error: %hs\n", ec.message().c_str());
    }
}
