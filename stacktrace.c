#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct Backtrace
{
    void **array;
    size_t size;
};

struct Backtrace s3();

int wmain()
{
    struct Backtrace bt = s3();

    wprintf(L"size of backtrace: %zu\n", bt.size);
    wprintf(L"backtrace:\n");
    for (size_t i = 0; i < bt.size; ++i)
    {
        wprintf(L" > 0x%zX\n", (uintptr_t)bt.array[i]);
    }

    return 0;
}
