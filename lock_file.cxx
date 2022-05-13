#include <stdio.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>

int wmain(int argc, wchar_t **argv)
{
    DWORD access = FILE_READ_ATTRIBUTES;
    DWORD sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;

    int locks_acquired = 0;
    bool ignore_flags = false; // after --, single argument treated as file
    for (int i = 1; i < argc; ++i)
    {
        const wchar_t *current_arg = argv[i];
        if (!ignore_flags && current_arg[0] == L'-')
        {
            if (current_arg[1] != L'-')
            {
                bool has_r = false;
                bool has_w = false;
                for (auto it = current_arg + 1; *it != L'\0'; ++it)
                {
                    if (*it == L'r' || *it == L'R')
                    {
                        access |= GENERIC_READ;
                        sharing &= ~FILE_SHARE_WRITE;
                    }
                    else if (*it == L'w' || *it == L'W')
                    {
                        access |= GENERIC_WRITE;
                        sharing = 0;
                    }
                    else
                    {
                        fwprintf(stderr, L"Unrecognized short option %lc\n", *it);
                        return 1;
                    }
                }
                continue;
            }

            // flag
            if (wcscmp(current_arg, L"--") == 0)
            {
                ignore_flags = true;
                continue;
            }
            if (wcscmp(current_arg, L"--read") == 0)
            {
                access |= GENERIC_READ;
                sharing &= ~FILE_SHARE_WRITE;
                continue;
            }
            if (wcscmp(current_arg, L"--write") == 0)
            {
                access |= GENERIC_WRITE;
                sharing = 0;
                continue;
            }
            fwprintf(stderr, L"Unrecognized option %ls\n", current_arg);
            return 1;
        }

        wprintf(L"CreateFileW(%ls, 0x%lX, 0x%lX)\n", current_arg, access, sharing);
        auto hnd = CreateFileW(current_arg, access, sharing, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hnd == INVALID_HANDLE_VALUE)
        {
            wprintf(L"  error: 0x%lX\n", GetLastError());
        }
        else
        {
            ++locks_acquired;
        }

        access = FILE_READ_ATTRIBUTES;
        sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
    }

    if (locks_acquired == 0)
    {
        wprintf(L"Failed to acquire any locks; exiting now.\n");
        return 1;
    }
    else
    {
        wprintf(L"Acquired %d lock(s); spinning forever\n", locks_acquired);
    }

    volatile bool continue_running = true;
    while (continue_running)
        ;
}
