#include <limits.h>
#include <stdio.h>
#include <windows.h>

bool parse(wchar_t const *s, unsigned long long &out)
{
    unsigned long long res = 0;

    for (auto it = s; *it != '\0'; ++it)
    {
        if (*it > '9' || *it < '0')
        {
            fwprintf(stderr, L"Could not parse number, string contains non-digit: %lc\n", *it);
            return false;
        }

        unsigned long long digit = *it - '0';
        // res * 10 + digit > ULLONG_MAX
        if (res > (ULLONG_MAX - digit) / 10)
        {
            fwprintf(stderr, L"Could not parse number, number too big: %ls > %llu\n", s, ULLONG_MAX);
        }
        res = res * 10 + digit;
    }

    out = res;
    return true;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc < 3)
    {
        fwprintf(stderr, L"Usage: %ls [file] [size]\n", argv[0]);
        return 1;
    }
    unsigned long long size;
    if (!parse(argv[2], size))
    {
        return 1;
    }

    HANDLE h = CreateFileW(argv[1], GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);

    if (h == INVALID_HANDLE_VALUE)
    {
        fwprintf(stderr, L"Could not open %ls: 0x%X\n", argv[1], GetLastError());
        return 1;
    }

    FILE_END_OF_FILE_INFO end_of_file;
    end_of_file.EndOfFile.QuadPart = static_cast<LONGLONG>(size);
    if (!SetFileInformationByHandle(h, FileEndOfFileInfo, &end_of_file, sizeof(end_of_file)))
    {
        fwprintf(stderr, L"Could not resize file %ls to %lld: 0x%X\n", argv[1], size, GetLastError());
        return 1;
    }

    wprintf(L"Successfully resized file %ls to %lld\n", argv[1], size);
    return 0;
}
